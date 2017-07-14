clc
clear all
close all

% experiment 1 used a different signal,  so cannot be used for data
% analysis

currentdir = pwd;

n_exp = [2:16];
%d_experiments = [4:15];
%s_experiments = [2 3 16]; % ATTENTION! e16 was done with only 1 subject sitting at robot1

experiments_struct = struct;
tic
for ii = n_exp
    cd(currentdir);
    Experiment = extract_parameters(ii);
    experiments_struct.Experiment{ii} = Experiment;
        if ii == 16
        experiments_struct.Experiment{ii}.e_r2 = experiments_struct.Experiment{ii}.e_r2.*NaN;
        experiments_struct.Experiment{ii}.improv_single_r2 = experiments_struct.Experiment{ii}.improv_single_r2.*NaN;
        experiments_struct.Experiment{ii}.F_r2 = experiments_struct.Experiment{ii}.F_r2.*NaN;
        %experiments_struct.Experiment{ii}.expfit_r2 = experiments_struct.Experiment{ii}.expfit_r2.*NaN;
        %experiments_struct.Experiment{ii}.pwrfit_r2 = experiments_struct.Experiment{ii}.pwrfit_r2.*NaN;
    end
end
toc
save('ALLresults.mat')