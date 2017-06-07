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

load('trialssequence.mat')
c_stiff = 0; %it can be 0(solo), or other 4 values

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    %%%   CHANGE FILENAME!!!   %%%
% filename
filename = 'expprotocol_bros_mattia_FIRSTTRY';

% create (main) struct
s = struct;

%% trial data
% for normal experiment

numTrials = 60; %number of trials
connected = seq(:, randi(5)); %2 subjects connected - using the 5 default sequencies
connectionStiffness = c_stiff*ones(1,numTrials); %which connection stiffness is used?
without = zeros(10,1);
with    = ones(20,1);
condition = [without; with; with; without]; % 1 is when there is the visuomotor rotation
trialDuration = 40 * ones(size(connected));
breakDuration = 15 * ones(size(connected));

% sort elements of trialRandomization in random order
phaseSets = [1:20, 1:20, 1:20];
trialRandomization = phaseSets(randperm(length(phaseSets)));

%numTrials = length(connected);

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
divTrials = {1:10; 11:30; 31:50; 51:60}; 
numBlocks = length(divTrials);

for ii = 1:numBlocks
    s.experiment.block{ii}.breakDuration = 240.0;
    s.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrials{ii})
        s.experiment.block{ii}.trial{jj} = trial{divTrials{ii}(jj)};
    end
end

%% write to to XML file

struct2xml(s,[filename '.xml']);

%% and save structure for further use in matlab

saving_filename = sprintf('%s.mat', filename);
eval 'save(saving_filename, 's');
