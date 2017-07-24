function [s, e_r1, e_r2] = extract_parameters (nExp)
clc
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                     INPUTS !!!
%
%
% Once you have set the basicdatapath to the folder where all your
% experiment folders are, you just need to change nEXp. This will change
% folder and will save the parameters of the experiment as well as an error
% figure in the correct folder.
%
%
% e16: Only robot 1 was used: thus, data from robot 2 is not needed
%
Expstring = sprintf('e%d', nExp);

fs = 1000; % this should always be the same because it's a robot thing

basicdatapath = '/media/mattia/DocPersonali/UNI/master-thesis/Experiment/data/Experiments/';

datapath = strcat(basicdatapath,Expstring); %where you .mat data is

currentdir = pwd;

cd(datapath); % go to data directory

basicinfo = '_info.mat';
fullinfo = strcat(Expstring, basicinfo);

load(fullinfo) % the file saved when generating the experimental protocol
load('data_trials.mat')

%%

n_trials = 0;
trial_sequence = [];
phase_sets = [];

for ii = 2:length(s.experiment.block)
    n_trials = n_trials + length(s.experiment.block{1,ii}.trial);
    for jj = 1:length(s.experiment.block{1,ii}.trial)
        trial_sequence = [trial_sequence; s.experiment.block{1,ii}.trial{1,jj}.connected];
        phase_sets = [phase_sets; s.experiment.block{1,ii}.trial{1,jj}.trialRandomization];
    end
end

trial_duration = s.experiment.block{1, 1}.trial{1, 1}.trialDuration; %this assumes that all the trials have the same duration
dyad = sum(trial_sequence);

%%

target_mat_r1 = NaN(trial_duration*fs, (n_trials)*2);
target_mat_r2 = NaN(trial_duration*fs, (n_trials)*2);
cursor_mat_r1 = NaN(trial_duration*fs, (n_trials)*2);
cursor_mat_r2 = NaN(trial_duration*fs, (n_trials)*2);
forces_mat_r1 = NaN(trial_duration*fs, (n_trials)*6);
forces_mat_r2 = NaN(trial_duration*fs, (n_trials)*6);

% create new time vector for resampling
tnew = 0:1/fs:45;
index = 1;
indexF = 1;

for i = 2:n_trials+1 %I want to avoid the first trial
    
    % resample
    target_BROS1_resampled = interp1(data.trial(i).t, data.trial(i).target_BROS1, tnew, 'linear','extrap');
    target_BROS2_resampled = interp1(data.trial(i).t, data.trial(i).target_BROS2, tnew, 'linear','extrap');
    cursor_BROS1_resampled = interp1(data.trial(i).t, data.trial(i).cursor_BROS1, tnew, 'linear','extrap');
    cursor_BROS2_resampled = interp1(data.trial(i).t, data.trial(i).cursor_BROS2, tnew, 'linear','extrap');
    forces_BROS1_resampled = interp1(data.trial(i).t, data.trial(i).ForcesOpSpace_BROS1, tnew, 'linear','extrap');
    forces_BROS2_resampled = interp1(data.trial(i).t, data.trial(i).ForcesOpSpace_BROS2, tnew, 'linear','extrap');
    
    target_mat_r1(:,index:index+1) = target_BROS1_resampled(5001:end-1,:);
    target_mat_r2(:,index:index+1) = target_BROS2_resampled(5001:end-1,:);
    
    cursor_mat_r1(:,index:index+1) = cursor_BROS1_resampled(5001:end-1,:);
    cursor_mat_r2(:,index:index+1) = cursor_BROS2_resampled(5001:end-1,:);
    
    forces_mat_r1(:,indexF:indexF+5) = forces_BROS1_resampled(5001:end-1,:);
    forces_mat_r2(:,indexF:indexF+5) = forces_BROS2_resampled(5001:end-1,:);
    
    index = index+2;
    indexF = indexF + 6;
end

trials = 1:n_trials;
trials = trials';

%% extracting parameters and saving structure

s = struct; %Here all the data from the experiment will be saved

cd(currentdir);

savingname = '_parameters';
saveparameters = strcat(Expstring,savingname);

errorstring = '_errors.fig';
errorstringpng = '_errors.png';
saveerrors = strcat(Expstring, errorstring);
saveEpng = strcat(Expstring, errorstringpng);

if dyad
    
    [e_r1, improv_single_trial_r1, improv_dual_trial_r1, e_r2, improv_single_trial_r2, improv_dual_trial_r2, relative_performance1] = calc_parameters_dyad (target_mat_r1, cursor_mat_r1, target_mat_r2, cursor_mat_r2, trial_sequence);
    %relative_performance1 is to be plotted with improvement data from
    %1, to plot data from 2 just take minus relative_performance1
    
    % normalization
    min_e_r1 = min(e_r1);
    min_e_r2 = min(e_r2);
    normd_e_r1 = e_r1 - min_e_r1;
    normd_e_r2 = e_r2 - min_e_r2;
    
    
    
    %     % figure
    %     figure
    %     plot(1:n_trials, normd_e_r1*100, 'o', 1:n_trials, normd_e_r2*100, 'o')
    %     savefig(saveerrors)
    %     saveas(gcf,saveEpng)
    
    % curve fitting
    expfit_r1 = fit(trials, normd_e_r1', 'exp1', 'Exclude', normd_e_r1>0.015);
    expfit_r2 = fit(trials, normd_e_r2', 'exp1', 'Exclude', normd_e_r2>0.015);
    pwrfit_r1 = fit(trials, normd_e_r1', 'power1', 'Exclude', normd_e_r1>0.015);
    pwrfit_r2 = fit(trials, normd_e_r2', 'power1', 'Exclude', normd_e_r2>0.015);
    
    %     cd(datapath); % go to data directory
    %     save(saveparameters, 'e_r1', 'e_r2', 'improv_dual_trial_r1', 'improv_single_trial_r1', 'improv_dual_trial_r2', 'improv_single_trial_r2', 'relative_performance1', 'trial_sequence', 'expfit_r1', 'expfit_r2', 'pwrfit_r1', 'pwrfit_r2', 'phase_sets')
    
    % save to structure
    s.e_r1 = normd_e_r1;
    s.e_r2 = normd_e_r2;
    s.improv_single_r1 = improv_single_trial_r1;
    s.improv_single_r2 = improv_single_trial_r2;
    s.improv_dual_r1 = improv_dual_trial_r1;
    s.improv_dual_r2 = improv_dual_trial_r2;
    s.relative_perf_r1 = relative_performance1;
    s.relative_perf_r2 = -relative_performance1;
    s.expfit_r1 = expfit_r1;
    s.pwrfit_r1 = pwrfit_r1;
    s.expfit_r2 = expfit_r2;
    s.pwrfit_r2 = pwrfit_r2;
    s.connected_sequence = trial_sequence;
    s.phases_sequence = phase_sets;
    s.F_r1 = forces_mat_r1;
    s.F_r2 = forces_mat_r2;
    
    
else
    
    [e_r1, improv_single_trial_r1] = calc_parameters_solo (target_mat_r1, cursor_mat_r1);
    [e_r2, improv_single_trial_r2] = calc_parameters_solo (target_mat_r2, cursor_mat_r2);
    
    
    % normalization
    min_e_r1 = min(e_r1);
    min_e_r2 = min(e_r2);
    normd_e_r1 = e_r1 - min_e_r1;
    normd_e_r2 = e_r2 - min_e_r2;
    
    %     %figure
    %     figure
    %     plot(1:n_trials, normd_e_r1*100, 'o', 1:n_trials, normd_e_r2*100, 'o')
    %     savefig(saveerrors)
    %     saveas(gcf,saveEpng)
    
    % curve fitting
    expfit_r1 = fit(trials, normd_e_r1', 'exp1', 'Exclude', normd_e_r1>0.015);
    expfit_r2 = fit(trials, normd_e_r2', 'exp1', 'Exclude', normd_e_r2>0.015);
    pwrfit_r1 = fit(trials, normd_e_r1', 'power1', 'Exclude', normd_e_r1>0.015);
    pwrfit_r2 = fit(trials, normd_e_r2', 'power1', 'Exclude', normd_e_r2>0.015);
    
    %     cd(datapath); % go to data directory
    %     save(saveparameters, 'e_r1', 'improv_single_trial_r1', 'e_r2', 'improv_single_trial_r2', 'expfit_r1', 'expfit_r2', 'pwrfit_r1', 'pwrfit_r2', 'phase_sets')
    
    % save to structure
    s.e_r1 = normd_e_r1;
    s.e_r2 = normd_e_r2;
    s.improv_single_r1 = improv_single_trial_r1;
    s.improv_single_r2 = improv_single_trial_r2;
    s.expfit_r1 = expfit_r1;
    s.pwrfit_r1 = pwrfit_r1;
    s.expfit_r2 = expfit_r2;
    s.pwrfit_r2 = pwrfit_r2;
    s.phases_sequence = phase_sets;
    s.F_r1 = forces_mat_r1;
    s.F_r2 = forces_mat_r2;
end


