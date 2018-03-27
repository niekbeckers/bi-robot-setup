%% createBROSExperimentProtocol_template
% Niek Beckers
% May 2017

clear all; close all; clc;

% NOTE: pair 6 was collected using the wrong model (TC), excluded. 

pairNr = 11;
selectPremadeTrialSequence = 1;
groupType = 'solo'; % solo or interaction
groupTypeNr = 0; % 0 = solo, 1 = interaction
Ks = 150;
Ds = 2;
expID = ['learning_pair' num2str(pairNr) '_session1_type' num2str(groupTypeNr)];

% filename
protocolpath = 'exp1b_protocols';
filename = ['protocol_' expID];

% create (main) struct
s = struct;

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
condition = [zeros(21,1); 2*ones(21,1); 2*ones(21,1); 2*ones(21,1)];
    
numTrials = numel(condition); % example
breakDuration = 10*ones(numTrials,1);
trialDuration = 20*ones(numTrials,1);

% connection
if strcmpi(groupType,'solo')
    connected = zeros(numTrials,1);
    connectionStiffness = zeros(numTrials,1);
    connectionDamping = zeros(numTrials,1);
elseif strcmpi(groupType,'interaction')
    connected = zeros(21,1); connected(2:2:end) = 1;
    connected = repmat(connected,4,1);

%     connected1 = zeros(42,1); connected1(2:2:end) = 1;
%     connected = [connected1;connected;connected];
    connectionStiffness = connected*Ks;
    connectionDamping = connected*Ds;

end

% specify how the trials are divided over the blocks
divTrials = {1:21 22:42 43:63 64:84};


%% randomization
% sort elements of trialRandomization in random order
if ~selectPremadeTrialSequence
    imin = 0;
    imax = 5;
    orderPerBlock = [repmat(imin:imax,1,3) randi([imin imax],1,3)]; 
    NRandomizationBlocks = {orderPerBlock, orderPerBlock, orderPerBlock, orderPerBlock};

    % %mix up the order:
    % nRand = [];
    % for ii = 1:length(NRandomizationBlocks)
    %     nRand = [nRand; NRandomizationBlocks{ii}(randperm(length(NRandomizationBlocks{ii})))];
    % end

    parfor ii = 1:4
        noconsnumbers = 1;
        while (noconsnumbers)
            randomOrder = NRandomizationBlocks{ii}(randperm(length(NRandomizationBlocks{ii})));
            if ~any(diff(randomOrder) == 0)
                noconsnumbers = 0;
                NRandomizationBlocks{ii} = randomOrder;
                disp(['done ' num2str(ii)])
            end
        end
    end
    
    % randomization: four target rotations, make sure that in each block, you
    % have the same amount of trials per rotation
    trialRandomization = 20*nRand+5*rand(size(nRand));

else
    load('exp1A_randomizedtrialorder_motorlearning_session1.mat');
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
%     if ~connected(ii)
%         trial{ii}.fitVirtualPartner.id0 = 1;
% %         trial{ii}.fitVirtualPartner.id1 = 2;
%     end
end

%% block data
% indicate how the trials are divided over the blocks
% NOTE: you always need at least 1 block
numBlocks = length(divTrials);
for ii = 1:numBlocks
    s.experiment.block{ii}.breakDuration = 240.0;
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