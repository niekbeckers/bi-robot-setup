%% createBROSExperimentProtocol_template
%
% TEMPLATE! DO NOT CHANGE (EXCEPT FOR BUG FIXES). COPY AND MAKE YOUR OWN
% SCRIPT FOR YOUR EXPERIMENT%
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

expID = 'tracking_pilot2_shortT';

% filename
filename = ['expprotocol_bros_' expID];

% create (main) struct
s = struct;

% indicate which type of trial feedback
s.experiment.expID = expID;
s.experiment.trialFeedback = 1;

%% trial data
numTrials = 10; % example

% trial settings
connected = false*[ones(numTrials,1)];
connectionStiffness = 0*ones(size(connected));
connectionDamping = 0*ones(size(connected));
condition = [zeros(10,1)];
breakDuration = 4*ones(size(connected));
trialDuration = 20*ones(size(connected));

% sort elements of trialRandomization in random order
trialRandomization = 80*rand(size(connected));

for ii = 1:numTrials
    trial{ii}.connected = connected(ii);
    trial{ii}.connectionStiffness = connectionStiffness(ii);
    trial{ii}.condition = condition(ii);
    trial{ii}.trialDuration = trialDuration(ii);
    trial{ii}.breakDuration = breakDuration(ii);
    trial{ii}.trialRandomization = trialRandomization(ii);
end

%% block data
% indicate how the trials are divided over the blocks
% NOTE: you always need at least 1 block

% indices of trials per block
divTrials = {1:10}; 
numBlocks = length(divTrials);


for ii = 1:numBlocks
    s.experiment.block{ii}.breakDuration = 30.0;
    s.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrials{ii})
        s.experiment.block{ii}.trial{jj} = trial{divTrials{ii}(jj)};
    end
end

%% save everything (in xml and mat)

% write to to XML file
struct2xml(s,[filename '.xml']);
% save experiment struct in mat file
save(filename, 's');