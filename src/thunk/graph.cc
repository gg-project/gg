/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 sw=2 tw=80 : */

#include "graph.hh"

#include <algorithm>
#include <iostream>
#include <queue>
#include <stdexcept>

#include "ggutils.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

std::ostream& operator<<( std::ostream& o, const ComputationKind& k )
{
  switch ( k ) {
    case ComputationKind::LINK:
      return o << "LINK";
    case ComputationKind::THUNK:
      return o << "THUNK";
    case ComputationKind::VALUE:
      return o << "VALUE";
    default:
      throw runtime_error( "Unknown ComputationKind" );
  }
}

Computation::Computation( Thunk&& thunk )
  : thunk( move( thunk ) )
{}

ComputationKind Computation::kind() const
{
  if ( outputs.size() ) {
    return ComputationKind::VALUE;
  } else if ( is_link_ ) {
    return ComputationKind::LINK;
  } else {
    return ComputationKind::THUNK;
  }
}

bool Computation::is_reducible_from_hash( const string& hash ) const
{
  return kind() == ComputationKind::THUNK and up_to_date
         and thunk.hash() == hash;
}

ExecutionGraph::ExecutionGraph( const bool log_renames )
  : verbose( getenv( "GG_VERBOSE" ) != nullptr )
  , log_renames_( log_renames )
{}

HashSet ExecutionGraph::add_thunk( const Hash& full_hash )
{
  if ( verbose )
    cerr << "Adding thunk: " << full_hash << endl;
  HashSet o1s;
  ComputationId _id = _add_thunk_common( full_hash, o1s );
  for ( const auto& o1 : o1s ) {
    if ( verbose )
      cerr << "  exec " << ids_.at( o1 ) << " as " << o1 << endl;
    ThunkWriter::write( computations_.at( ids_.at( o1 ) ).thunk );
  }
  return o1s;
}

ComputationId ExecutionGraph::_add_thunk_common( const Hash& full_hash,
                                                 HashSet& o1s )
{
  // Strip output name
  const string hash = gg::hash::base( full_hash );

  // If hash is present, we're done
  if ( ids_.count( hash ) ) {
    if ( verbose )
      cerr << "  duplicate: " << ids_.at( hash ) << endl;
    return ids_.at( hash );
  }

  // Otherwise, get the hash's thunk
  Thunk thunk { ThunkReader::read( gg::paths::blob( hash ), hash ) };

  // Add blob dependencies
  for ( const Thunk::DataItem& item : thunk.values() ) {
    blob_dependencies_.emplace( item.first );
  }
  for ( const Thunk::DataItem& item : thunk.executables() ) {
    blob_dependencies_.emplace( item.first );
  }

  // Build node in graph
  ComputationId id = next_id_++;
  auto r = computations_.emplace( id, Computation { move( thunk ) } );
  Computation& computation = ( r.first )->second;
  ids_.emplace( hash, id );

  n_thunks_++;

  // Add dependencies, registering O(1) dependencies
  for ( const Thunk::DataItem& item : computation.thunk.thunks() ) {
    const string child_hash = gg::hash::base( item.first );
    ComputationId child_id = _add_thunk_common( child_hash, o1s );
    _create_dependency( id, child_hash, child_id );
  }
  for ( const Thunk::DataItem& item : computation.thunk.futures() ) {
    const string child_hash = gg::hash::base( item.first );
    ComputationId child_id = _add_thunk_common( child_hash, o1s );
    _create_dependency( id, child_hash, child_id );
  }

  // Register us, if we're O(1)
  if ( computation.thunk.can_be_executed() ) {
    o1s.insert( computation.thunk.hash() );
  }

  // Update our thunk.
  _update( id );

  if ( verbose ) {
    cerr << "  new id: " << id << " deps: ";
    for ( const auto& dep : computation.deps ) {
      cerr << dep << " ";
    }
    cerr << endl;
  }

  return id;
}

void ExecutionGraph::_mark_out_of_date( const ComputationId id )
{
  Computation& computation = computations_.at( id );
  if ( not computation.is_value() and computation.up_to_date ) {
    computation.up_to_date = false;
    for ( const ComputationId parent_id : computation.rev_deps ) {
      _mark_out_of_date( parent_id );
    }
  }
}

IdSet ExecutionGraph::_update( const ComputationId id )
{
  if ( verbose )
    cerr << "      _update(" << id << ")" << endl;

  Computation& computation = computations_.at( id );

  if ( computation.up_to_date ) {
    return {};
  }

  IdSet newly_executable;

  switch ( computation.kind() ) {
    case ComputationKind::THUNK: {
      // update all dependencies, and update our hash-pointer to them.
      const auto deps { computation.deps };
      for ( const ComputationId child_id : deps ) {
        IdSet r = _update( child_id );
        newly_executable.insert( r.begin(), r.end() );
        Computation& child = computations_.at( child_id );
        string old_hash = computation.dep_hashes.at( child_id );
        if ( child.is_value() ) {
          string old_p_hash;
          if ( log_renames_ ) {
            old_p_hash = computation.thunk.hash();
          }
          computation.thunk.update_data( old_hash, child.outputs );
          if ( log_renames_ ) {
            const string& new_p_hash = computation.thunk.hash();
            roost::atomic_create( new_p_hash, paths::rename( old_p_hash ) );
          }
          _erase_dependency( id, child_id );
        } else {
          string new_hash = child.thunk.hash();
          if ( old_hash != new_hash ) {
            string old_p_hash;
            if ( log_renames_ ) {
              old_p_hash = computation.thunk.hash();
            }
            computation.thunk.update_data( old_hash, { { new_hash, "" } } );
            if ( log_renames_ ) {
              const string& new_p_hash = computation.thunk.hash();
              roost::atomic_create( new_p_hash, paths::rename( old_p_hash ) );
            }
            computation.dep_hashes[child_id] = new_hash;
          }
        }
      }

      // Compute our new hash.
      const string& hash = computation.thunk.hash();

      // Check if we're a link.
      if ( ids_.count( hash ) ) {
        if ( ids_.at( hash ) != id ) {
          computation.is_link_ = true;
          _cut_dependencies( id );
          _create_dependency( id, hash, ids_.at( hash ) );
          IdSet r = _update( id );
          newly_executable.insert( r.begin(), r.end() );
        }
      } else {
        ids_.emplace( hash, id );
      }
      break;
    }
    case ComputationKind::LINK: {
      // Set our value if our child has a value.
      ComputationId child_id = *computation.deps.begin();
      Computation& child = computations_.at( child_id );
      IdSet r = _update( child_id );
      newly_executable.insert( r.begin(), r.end() );
      if ( child.is_value() ) {
        computation.is_link_ = false;
        computation.outputs = child.outputs;
        assert( computation.deps.size() == 1 );
        _erase_dependency( id, child_id );
        assert( computation.deps.empty() );
      }
      break;
    }
    case ComputationKind::VALUE:
    default: {
      // empty
    }
  }

  if ( computation.is_thunk() and computation.thunk.can_be_executed() ) {
    newly_executable.insert( id );
  }

  computation.up_to_date = true;
  return newly_executable;
}

IdSet ExecutionGraph::_update_parent_thunks( const ComputationId id )
{
  std::queue<ComputationId> to_update;
  IdSet updated;
  for ( const ComputationId parent_id : computations_.at( id ).rev_deps ) {
    if ( verbose )
      cerr << "    maybe update " << parent_id << endl;
    to_update.push( parent_id );
  }
  while ( not to_update.empty() ) {
    const ComputationId id = to_update.front();
    to_update.pop();
    Computation& computation = computations_.at( id );
    if ( verbose )
      cerr << "    check " << id << endl;
    switch ( computation.kind() ) {
      case ComputationKind::LINK: {
        if ( verbose )
          cerr << "      is link " << endl;
        IdSet r = _update( id );
        if ( verbose ) {
          for ( const ComputationId parent_id : r ) {
            cerr << "    updated " << parent_id << endl;
          }
        }
        updated.insert( r.begin(), r.end() );
        for ( const ComputationId parent_id : computation.rev_deps ) {
          if ( verbose )
            cerr << "    maybe update " << parent_id << endl;
          to_update.push( parent_id );
        }
        break;
      }
      case ComputationKind::THUNK: {
        if ( verbose )
          cerr << "      is thunk " << endl;
        if ( not computation.up_to_date ) {
          if ( verbose )
            cerr << "      out of date" << endl;
          IdSet r = _update( id );
          if ( verbose ) {
            for ( const ComputationId parent_id : r ) {
              cerr << "    updated " << parent_id << endl;
            }
          }
          updated.insert( r.begin(), r.end() );
          if ( not computation.is_thunk() ) {
            for ( const ComputationId parent_id : computation.rev_deps ) {
              if ( verbose )
                cerr << "    maybe update " << parent_id << endl;
              to_update.push( parent_id );
            }
          }
        }
        break;
      }
      case ComputationKind::VALUE:
      default: {
        std::cerr << ( "Computation " + to_string( id )
                       + " is a value in _update_parent_thunks" )
                  << std::endl;
        break;
      }
    }
  }
  return updated;
}

void ExecutionGraph::_create_dependency( const ComputationId from,
                                         const Hash& on_hash,
                                         const ComputationId on )
{
  Computation& parent = computations_.at( from );
  Computation& child = computations_.at( on );
  _update( on );
  if ( not child.up_to_date ) {
    throw runtime_error( "cannot depend on out-of-date computations" );
  }
  if ( parent.dep_hashes.count( on ) > 0
       and parent.dep_hashes.at( on ) != on_hash ) {
    throw runtime_error(
      to_string( from ) + " depended on computation #" + to_string( on )
      + " for hash " + parent.dep_hashes.at( on )
      + " but now it should depend on it for hash " + on_hash );
  }

  parent.dep_hashes[on] = on_hash;
  parent.deps.insert( on );
  child.rev_deps.insert( from );
}

std::vector<Hash> ExecutionGraph::_erase_dependency( const ComputationId from,
                                                     const ComputationId on )
{
  Computation& parent = computations_.at( from );
  Computation& child = computations_.at( on );
  parent.deps.erase( on );
  parent.dep_hashes.erase( on );
  child.rev_deps.erase( from );
  return _remove_if_unneeded( on );
}

std::vector<Hash> ExecutionGraph::_cut_dependencies( const ComputationId id )
{
  std::vector<Hash> removed;
  Computation& computation = computations_.at( id );
  for ( const ComputationId child : computation.deps ) {
    computations_.at( child ).rev_deps.erase( id );
    const auto r = _remove_if_unneeded( child );
    removed.insert( removed.end(), r.begin(), r.end() );
  }
  computation.deps.clear();
  computation.dep_hashes.clear();
  return removed;
}

std::vector<Hash> ExecutionGraph::_remove_if_unneeded( const ComputationId id )
{
  std::vector<Hash> removed;
  Computation& computation = computations_.at( id );
  if ( computation.is_thunk() and computation.rev_deps.size() == 0 ) {
    if ( computation.is_thunk() ) {
      n_thunks_--;
    }
    const auto deps = computation.deps;
    const auto r = _cut_dependencies( id );
    removed.insert( removed.end(), r.begin(), r.end() );
    if ( computation.is_thunk() ) {
      removed.push_back( computation.thunk.hash() );
    }
    computations_.erase( id );
  }
  return removed;
}

ComputationId ExecutionGraph::_follow_links( ComputationId id ) const
{
  while ( computations_.at( id ).is_link() ) {
    id = *computations_.at( id ).deps.begin();
  }
  return id;
}

Optional<Hash> ExecutionGraph::query_value( const Hash& hash ) const
{
  if ( ids_.count( hash ) == 0 ) {
    return {};
  }
  const Computation& computation = computations_.at( ids_.at( hash ) );
  if ( computation.is_value() ) {
    return { true, computation.outputs.front().hash };
  } else {
    return {};
  }
}

std::pair<std::unordered_set<Hash>, std::vector<Hash>>
ExecutionGraph::submit_reduction( const Hash& from,
                                  std::vector<gg::ThunkOutput>&& to )
{
  if ( verbose )
    cerr << "Reduced " << from << " to " << to.front().hash << endl;
  // Assert non-empty output
  vector<ThunkOutput> outputs { move( to ) };
  if ( outputs.size() == 0 ) {
    throw runtime_error( "ExecutionGraph::submit_reduction: empty `to`" );
  }

  // If this is a self reduction, return "no new work"
  if ( from == outputs.front().hash ) {
    return {};
  }

  // Assert non-value initial hash
  if ( gg::hash::type( from ) == gg::ObjectType::Value ) {
    throw runtime_error(
      "ExecutionGraph::submit_reduction: cannot reduce value: " + from );
  }

  // If this hash is unknown, error
  if ( ids_.count( from ) == 0 ) {
    throw runtime_error(
      string( "ExecutionGraph::submit_reduction: cannot reduce unknown hash: " )
      + from );
  }

  ComputationId id = ids_.at( from );
  if ( verbose )
    cerr << "  id " << id << endl;

  // We've cancelled this computation.
  if ( computations_.count( id ) == 0 ) {
    if ( verbose )
      cerr << " discarding update: comptutation removed" << endl;
    return {};
  }

  Computation& computation = computations_.at( id );
  // If this reduction is for an out-of-date thunk, ignore it
  if ( not computation.is_reducible_from_hash( from ) ) {
    if ( verbose )
      cerr << " discarding update: old" << endl;
    return {};
  }

  // Finally, accept the reduction
  _mark_out_of_date( id );
  const auto removed = _cut_dependencies( id );
  assert( computation.deps.empty() );

  string& new_hash = outputs.front().hash;
  const gg::ObjectType new_type = gg::hash::type( new_hash );

  HashSet new_o1s;
  if ( new_type == gg::ObjectType::Thunk ) {
    // A new thunk -- accumulate leaf dependencies
    ComputationId new_id = _add_thunk_common( new_hash, new_o1s );
    computation.is_link_ = true;
    _create_dependency( id, new_hash, new_id );
    _update( id );
  } else {
    // A value -- set outputs
    computation.outputs = move( outputs );
  }
  n_thunks_--;

  // If we found no O(1) deps looking downward, look upward
  if ( new_o1s.size() == 0 ) {
    IdSet newly_updated = _update_parent_thunks( id );
    for ( const ComputationId ancestor_id : newly_updated ) {
      Computation& ancestor = computations_.at( ancestor_id );
      if ( ancestor.thunk.can_be_executed() ) {
        // Make sure to record the hash and write the thunk before submitting
        new_o1s.insert( ancestor.thunk.hash() );
      }
    }
  }

  for ( const auto& o1 : new_o1s ) {
    if ( verbose ) {
      cerr << "  exec " << ids_.at( o1 ) << " as " << o1 << endl;
    }
    ThunkWriter::write( computations_.at( ids_.at( o1 ) ).thunk );
  }
  return { new_o1s, removed };
}
