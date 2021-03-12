mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

lambda_bins:
	env DOCKER_BUILDKIT=1 docker build --output tools/lambda_bins . && chmod +x tools/lambda_bins*
lambda_upload:
	cd $(mkfile_dir)/src/remote && python3 $(mkfile_dir)/src/remote/create-function.py --delete --gg-execute-static=$(mkfile_dir)/tools/lambda_bins/gg-execute-lambda --toolchain-path=$(mkfile_dir)/toolchain

