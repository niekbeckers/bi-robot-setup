%% createBROSExperimentProtocol_template
% Niek Beckers
% May 2017

clear all; close all; clc;

pairNr = 104;
groupType = 'interaction'; % solo or interaction
groupTypeNr = 1; % 0 = solo, 1 = interaction
Ks = 125;
Ds = 5;
expID = ['id_pair' num2str(pairNr) '_type' num2str(groupTypeNr)];

% filename
protocolpath = 'exp3_protocols';
filename = ['protocol_pilot_2d_compensatory_' expID];

% create (main) struct
s = struct;

% display type (pursuit or compensatory)
s.experiment.displayType = 3; % 0 = pursuit, 1 = pursuit_1d, 2 = compensatory, 3 = compensatory_2d
s.experiment.cursorShape = 3; % 0 = circle, 1 = line, 2 = cross


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
condition = [3*ones(2,1); 3*ones(11,1)];
    
numTrials = numel(condition); % example
breakDuration = 15*ones(numTrials,1);
trialDuration = 90*ones(numTrials,1);

% connection
if strcmpi(groupType,'solo')
    connected = zeros(numTrials,1);
    connectionStiffness = zeros(numTrials,1);
    connectionDamping = zeros(numTrials,1);
elseif strcmpi(groupType,'interaction')
    connected = zeros(12,1); connected(4:2:end) = 1;
    connected = repmat(connected,4,1);

%     connected1 = zeros(42,1); connected1(2:2:end) = 1;
%     connected = [connected1;connected;connected];
    connectionStiffness = connected*Ks;
    connectionDamping = connected*Ds;
end

% specify how the trials are divided over the blocks
divTrials = {1:2 3:13};


%% randomization
trialRandomization = zeros(numTrials,1);

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
numBlocks = length(divTrials);
for ii = 1:numBlocks
    s.experiment.block{ii}.breakDuration = 120.0;
    s.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrials{ii})
        s.experiment.block{ii}.trial{jj} = trial{divTrials{ii}(jj)};
    end
    
    expprotocol.block(ii).connected = connected(divTrials{ii});
    expprotocol.block(ii).condition = condition(divTrials{ii});
    expprotocol.block(ii).trialRandomization = trialRandomization(divTrials{ii});
    expprotocol.block(ii).connectionStiffness = connectionStiffness(divTrials{ii});
    expprotocol.block(ii).connectionDamping = connectionDamping(divTrials{ii});
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