#!/usr/bin/env bash
conda create -y -n tracker opencv=3.1 boost=1.53 cmake &&
conda activate tracker &&
conda install -c conda-forge dlib &&
tar xzf conda_path.tar.gz -C $CONDA_PREFIX &&
conda deactivate &&
echo "Conda configured. To start using the environmnet type 'conda activate tracker'"

