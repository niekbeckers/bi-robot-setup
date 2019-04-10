%% createBROSExperimentProtocol_template
% Niek Beckers
% May 2017

clear all; close all; clc;

pairNr = 1;
groupType = 'interaction'; % solo or interaction
groupTypeNr = 1; % 0 = solo, 1 = interaction
Ks = 107;
Ds = 6;
expID = ['idemg'];

% filename
protocolpath = 'exp4_protocols';
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
condition = [3*ones(10,1); 3*ones(14,1); 3*ones(14,1); 3*ones(20,1); 3*ones(20,1); 3*ones(5,1)];
%     condition = [3*ones(10,1)];
    
numTrials = numel(condition); % example
breakDuration = 12*ones(numTrials,1);
trialDuration = 28*ones(numTrials,1);

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

% specify how the trials are divided over the blocks
divTrials = {1:10; 11:24; 25:38; 39:58; 59:78; 79:83};


%% randomization
if exist('exp4_protocols/exp4_randomization.mat','file')
    load('exp4_protocols/exp4_randomization.mat');
else 
    trialRandomization = 24*rand(size(condition));
    save('exp4_protocols/exp4_randomization.mat','trialRandomization');
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