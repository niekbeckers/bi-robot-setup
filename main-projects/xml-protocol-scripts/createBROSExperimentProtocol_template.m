%% createBROSExperimentProtocol_template
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
% Niek Beckers
% May 2017

clear all; close all; clc;

% filename
filename = 'experimentprotocol_bros_template';

% create (main) struct
s = struct;

%% trial data
connected = [1;0;1;0;1;0;1;0];
connectionStiffness = [60;0;60;0;60;0;60;0];
condition = ones(size(connected));
trialDuration = 40*ones(size(connected));
breakDuration = 6*ones(size(connected));
trialRandomization = [1;2;3;4;1;2;3;4];
numtrials = length(connected);

for ii = 1:numtrials
    trial{ii}.connected = connected(ii);
    trial{ii}.connectionStiffness = connectionStiffness(ii);
    trial{ii}.condition = condition(ii);
    trial{ii}.trialDuration = trialDuration(ii);
    trial{ii}.breakDuration = breakDuration(ii);
    trial{ii}.trialRandomization = trialRandomization(ii);
end


%% block data
seltrials = {1:4; 5:8};
numblocks = length(seltrials);

for ii = 1:numblocks
    s.experiment.block{ii}.breakDuration = 300.0;
    s.experiment.block{ii}.homingType = 302;
    for jj = 1:length(seltrials{ii})
        s.experiment.block{ii}.trial{jj} = trial{seltrials{ii}(jj)};
    end
end

%% write to to XML file
struct2xml(s,[filename '.xml']);