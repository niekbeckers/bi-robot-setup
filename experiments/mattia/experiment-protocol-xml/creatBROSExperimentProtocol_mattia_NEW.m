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

% filename
filename = 'expprotocol_bros_template_mattia_e28_Fdyad12';
load('sequencesNEW')

% create (main) struct
s = struct;

% set trial duration and break duration in seconds
trialD = 40;
breakD = 10;
betweenblocksD = 120.0;

%set connection stiffness and damping
c_stiffness = 250;
c_damping = 5;
%% trial data


numTrials = 49;

if c_stiffness
    connected = seq(:, randi(2))';
else
    connected = zeros(1,numTrials);
end
connected = [0 connected];
connectionStiffness = c_stiffness*ones(1,numTrials);
connectionDamping = c_damping*ones(1,numTrials);
condition = zeros(1,numTrials);
condition(1) = 1; %condition = 1 makes the easy signal to run
trialDuration = trialD*ones(size(connected));
breakDuration = breakD*ones(size(connected));

% sort elements of trialRandomization in random order
phaseSets = [1:4, 1:4, 1:4, 1:4, 1:4, 1:4, 1:4, 1:4, 1:4, 1:4, 1:4, 1:4];
trialRandomization = phaseSets(randperm(length(phaseSets)));
% add NaN to correspond with the first trial (the easy one)
trialRandomization = [1 trialRandomization];
%numTrials = length(connected);

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

% indices of trials per block

divTrials = {1; 2:9; 10:17; 18:25; 26:33; 34:41; 42:49};
numBlocks = length(divTrials);

s.experiment.trialFeedback = 1;
s.experiment.trialPerformanceThreshold = 0.15;

for ii = 1:numBlocks
    s.experiment.block{ii}.breakDuration = betweenblocksD;
    s.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrials{ii})
        s.experiment.block{ii}.trial{jj} = trial{divTrials{ii}(jj)};
    end
end

%% write to to XML file
struct2xml(s,[filename '.xml']);
save('e28_info', 's')