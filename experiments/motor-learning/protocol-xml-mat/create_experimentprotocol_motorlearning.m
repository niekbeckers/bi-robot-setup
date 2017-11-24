%% createBROSExperimentProtocol_template
%
% - Niek
%
%
% For this to work, you need to download struct2xml (https://nl.mathworks.com/matlabcentral/fileexchange/28639-struct2xml)
% The BROS GUI and experiment handler software reads a XML file with the
% following structure:
%
% <?xml version="1.0" encoding="utf-8"?>
% <experiment>
%    <countDownDuration>3</countDownDuration> 
%    <trialFeedback>0</trialFeedback>
%     <trialPerformanceThreshold>0.1</trialPerformanceThreshold>
%     or:
%     <trialMTRangeLower>0.8</trialMTRangeLower>
%     <trialMTRangeUpper>1.2</trialMTRangeUpper>
%    <block>
%       <breakDuration>300</breakDuration>
%       <homingType>302</homingType>
%       <trial>
%          <connected>1</connected>
%          <connectionStiffness>60</connectionStiffness>
%          <condition>1</condition>
%          <trialDuration>40</trialDuration>
%          <breakDuration>6</breakDuration>
%          <trialRandomization>1</trialRandomization>
%       </trial>
%       <trial>
%          ...
%       </trial>
%    </block>
%    <block>
%       <breakDuration>300</breakDuration>
%       <homingType>302</homingType>
%       <trial>
%          ...
%       </trial>
%    </block>
% </experiment>
%
% Most/some of the parameters per trial are sent to the simulink model
% through the TwinCAT ADS server.
% The names of the parameters have to match the following:
%
% - Parameters
%   - Experiment:
%       countDownDuration:      duration of countdown [s] (default: 3s)
%       trialFeedback:          trial performance feedback to user [-]
%                               0: no feedback
%                               1: Mean Squared Error
%                               2: Movement time
%                               Based on the performance trial feedback
%                               type, you can set lower, upper bounds to
%                               movement time or a MSE performance
%                               threshold. 
%       trialPerformanceThreshold: MSE threshold whether performance is
%                               better or worse than previous performance.
%       trialMTRangeLower       Movement time lower bound
%       trialMTRangeUpper       Movement time upper bound
%   - Trial: 
%       connected:              physical connection between BROS1 and BROS2 (bool)
%       connectionStiffness:    connection stiffness [N/m] (double)
%       connectionDamping:      connection damping [Ns/m] (double)
%       condition:              condition number, general use (int)
%       trialDuration:          optional (double)
%       breakDuration:          break duration (in between trial) [s] (double)
%       trialRandomization:     integer to select the phase sets e.g. (int)
%   - Block:
%       breakDuration:          optional break duration between blocks [s] (double)
%       homingType:             301: manual homing (go to home position), 302: automatic
%
% Niek Beckers
% May 2017

clear all; close all; clc;

partnersNr = 14;
sessionnr = 1;
selectPremadeTrialSequence = 1;
groupType = 'interaction'; % solo or interaction
groupTypeNr = 1; % 0 = solo, 1 = interaction
Ks = 150;
Ds = 2;
expID = ['motorlearning_partners' num2str(partnersNr) '_session' num2str(sessionnr) '_type' num2str(groupTypeNr)];

% filename
filename = ['expprotocol_' expID];

% create (main) struct
s = struct;

% indicate which type of trial feedback
s.experiment.expID = expID;
s.experiment.trialFeedback = 1;
s.experiment.trialPerformanceThreshold = 0.05;
s.experiment.groupTypeNr = groupTypeNr;
s.experiment.sessionNr = sessionnr;
s.experiment.partnersNr = partnersNr;

% s.experiment.doVirtualPartner = 0;
s.experiment.activeBROSID.id0 = 1;
s.experiment.activeBROSID.id1 = 2;

%% trial data


% trial settings

% experiment settings
switch sessionnr
    case 1
        condition = [zeros(21,1); ones(21,1); ones(21,1); zeros(21,1)];
    case 2
        condition = [zeros(21,1); ones(21,1); ones(21,1)];
end
    
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
switch sessionnr
    case 1
        divTrials = {1:21 22:42 43:63 64:84};
    case 2
        divTrials = {1:21 22:42 43:63};
end


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
    load('randomizedtrialorder_motorlearning_session1.mat');
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
%     trial{ii}.fitVirtualPartner.id0 = 1;
%     trial{ii}.fitVirtualPartner.id1 = 2;
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

protocolpath = 'protocols';

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