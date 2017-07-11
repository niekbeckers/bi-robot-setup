clc
clear all
close all

% experiment 1 used a different signals, so cannot be used for data
% analysis

currentdir = pwd;

n_exp = [2:16];
%d_experiments = [4:15];
%s_experiments = [2 3 16]; % ATTENTION! e16 was done with only 1 subject sitting at robot1

experiments_struct = struct;
tic
for ii = n_exp
    cd(currentdir);
    Experiment = run_1st_analysis(ii);
    experiments_struct.Experiment{ii} = Experiment;
end
toc