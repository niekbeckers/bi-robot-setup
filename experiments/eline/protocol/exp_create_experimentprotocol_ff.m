%% createBROSExperimentProtocol_template
% Niek Beckers
% May 2017

clear all; close all; clc;

pairNr = 1;
groupType = 'solo'; % solo or interaction
groupTypeNr = 0; % 0 = solo, 1 = interaction
Ks = 0;
Ds = 0;
expID = ['ffemg'];

% filename
protocolpath = 'exp_ffemg_protocols';
filename = ['protocol_' expID];

% create (main) struct
s = struct;

% display type (pursuit or compensatory)
s.experiment.displayType = 0; % 0 = pursuit, 1 = pursuit_1d, 2 = compensatory, 3 = compensatory_2d
s.experiment.cursorShape = 3; % 0 = circle, 1 = line, 2 = cross, 3 - crosshair


% indicate which type of trial feedback
s.experiment.expID = expID;
s.experiment.trialFeedback = 1;
s.experiment.trialPerformanceThreshold = 0.05;
s.experiment.groupTypeNr = groupTypeNr;
s.experiment.sessionNr = 1;
s.experiment.partnersNr = pairNr;

% s.experiment.doVirtualPartner = 0;
s.experiment.activeBROSID.id0 = 1;
s.experiment.activeBROSID.id1 = 2;

%% trial data

% trial settings

% experiment settings

% condition vector: each index is a trial
Nreps = 4;
cond_tmp = repmat((1:5).',Nreps,1);
cond_sorting = randperm(length(cond_tmp));
condition = [0*ones(Nreps,1); cond_tmp(cond_sorting)];

condition = [0;0;1;2;3;4;5;0];

% specify how the trials are divided over the blocks
divTrialsOverBlocks = {1:4; 5:24};
divTrialsOverBlocks = {1:length(condition)};
    
numTrials = numel(condition); % example
breakDuration = 15*ones(numTrials,1);
trialDuration = 30*ones(numTrials,1);

% connection
if strcmpi(groupType,'solo')
    connected = zeros(numTrials,1);
    connectionStiffness = zeros(numTrials,1);
    connectionDamping = zeros(numTrials,1);
elseif strcmpi(groupType,'interaction')
    connected = [zeros(10,1);  [1;0;1;0;1;0;1;0;1;0;1;0;1;0]; [1;0;1;0;1;0;1;0;1;0;1;0;1;0]; zeros(20,1); ones(20,1); zeros(5,1)];
    connectionStiffness = connected*Ks;
    connectionDamping = connected*Ds;
end


%% randomization
if exist('exp_ffemg_protocols/exp_ffemg_randomization.mat','file')
    load('exp_ffemg_protocols/exp_ffemg_randomization.mat');
else 
    trialRandomization = 30*rand(size(condition));
    save('exp_ffemg_protocols/exp_ffemg_randomization.mat','trialRandomization');
end

%% prepare trials
for ii = 1:numTrials
    trial{ii}.connected = connected(ii);
    trial{ii}.connectionStiffness = connectionStiffness(ii);
    trial{ii}.connectionDamping = connectionDamping(ii);
    trial{ii}.condition = condition(ii);
    trial{ii}.trialDuration = trialDuration(ii);
    trial{ii}.breakDuration = breakDuration(ii);
    trial{ii}.trialRandomization = trialRandomization(ii);
end

%% block data
% indicate how the trials are divided over the blocks
% NOTE: you always need at least 1 block
numBlocks = length(divTrialsOverBlocks);
for ii = 1:numBlocks
    s.experiment.block{ii}.breakDuration = 120.0;
    s.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrialsOverBlocks{ii})
        s.experiment.block{ii}.trial{jj} = trial{divTrialsOverBlocks{ii}(jj)};
    end
    
    expprotocol.block(ii).connected = connected(divTrialsOverBlocks{ii});
    expprotocol.block(ii).condition = condition(divTrialsOverBlocks{ii});
    expprotocol.block(ii).trialRandomization = trialRandomization(divTrialsOverBlocks{ii});
    expprotocol.block(ii).connectionStiffness = connectionStiffness(divTrialsOverBlocks{ii});
    expprotocol.block(ii).connectionDamping = connectionDamping(divTrialsOverBlocks{ii});
end



%% save everything (in xml and mat)

if ~exist(protocolpath,'dir')
    mkdir(protocolpath);
end

% check if you want to overwrite the protocol
saveProtocol = 1;
if exist([protocolpath filesep filename '.xml'],'file') || exist([protocolpath filesep filename],'file')
    promptMessage = sprintf('This file already exists:\n%s\nDo you want to overwrite it?', [protocolpath filesep filename '.xml']);
	titleBarCaption = 'Overwrite protocol?';
	buttonText = questdlg(promptMessage, titleBarCaption, 'Yes', 'No', 'No');
    
    if strcmpi(buttonText,'No')
        saveProtocol = 0;
    end
end

if saveProtocol
    % write to to XML file
    struct2xml(s,[protocolpath filesep filename '.xml']);
    % save experiment struct in mat file
    save([protocolpath filesep filename], 's','expprotocol');
end