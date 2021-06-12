mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

static_bins:
	env DOCKER_BUILDKIT=1 docker build -f Dockerfile-static --output tools/static_bins . && chmod +x tools/static_bins* && rm -rf tools/static_bins/*dir
lambda_bins:
	env DOCKER_BUILDKIT=1 docker build --output tools/lambda_bins . && chmod +x tools/lambda_bins*
lambda_upload:
	cd $(mkfile_dir)/src/remote && python3 $(mkfile_dir)/src/remote/create-function.py --delete --gg-execute-static=$(mkfile_dir)/tools/lambda_bins/gg-execute-lambda --toolchain-path=$(mkfile_dir)/toolchain

