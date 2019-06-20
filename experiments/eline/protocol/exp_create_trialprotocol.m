%% createBROstExperimentProtocol_template
% Niek Beckerst
% May 2017

clear all; close all; clc;

pairNr = 1;
groupType = 'interaction'; % stolo or interaction
groupTypeNr = 0; % 0 = stolo, 1 = interaction
Kdes = 100; 
Ks = 1/((1/Kdes)-(1/1500));
Ds = 3;
% expID = ['ffemg'];
expID = ['test_traj_conn_VN'];

% filename
% protocolpath = 'exp_ffemg_protocols';
protocolpath = 'exp_test_protocols';

filename = ['protocol_' expID];

% create (main) sttruct
st = struct;


% indicate which type of trial feedback
st.experiment.expID = expID;
st.experiment.trialFeedback = 1;
st.experiment.groupTypeNr = groupTypeNr;
st.experiment.sessionNr = 1;
st.experiment.partnersNr = pairNr;

% st.experiment.doVirtualPartner = 0;
st.experiment.activeBROSID.id0 = 1;
st.experiment.activeBROSID.id1 = 2;

% target stettingst
st.experiment.targetParticleCount = 5;
st.experiment.targetParticlePeriod = 0.5;% how long each child particle mopves before reset
st.experiment.targetParticleSize = 6.0; % size of partticle (px)

%% trial data

% trial stettingst

% experiment stettingst

% condition vector: each index is a trial
% condition to -1: baseline trial (no movement)

% Condition for ffemg trial 
% Nreps = 5;
% cond_tmp = repmat((1:5).',Nreps,1);
% cond_sorting = randperm(length(cond_tmp));
% condition = [-1*ones(6,1); zeros(10,1); cond_tmp(cond_sorting)];

% condition = zeros(10,1); 
condition = [-1*ones(5,1); zeros(28,1)]; 
% condition = [-1;0;1;2;3;4;5];

% % Condition for VN experiment 
% condition = zeros(1,60);

% stpecify how the trialst are divided over the blockst
% % Blocks for VN experiment 
% divTrialsOverBlocks = {1:15; 16:30; 31:45; 46:60};

% Blocks for ffemg trial 
% divTrialsOverBlocks = {1:6; 7:17; 18:28; 29:41};

divTrialsOverBlocks = {1:5; 6:33};

    
numTrials = numel(condition); % example
breakDuration = 5*ones(numTrials,1);
% trialDuration = 20*ones(numTrials,1);
trialDuration = [20; 30; 30; 30; 30; 20*ones(28,1)];


% connection
if strcmpi(groupType,'stolo')
    connected = zeros(numTrials,1);
    connectionStiffness = zeros(numTrials,1);
    connectionDamping = zeros(numTrials,1);
elseif strcmpi(groupType,'interaction')
%     connected = [zeros(10,1);  [1;0;1;0;1;0;1;0;1;0;1;0;1;0]; [1;0;1;0;1;0;1;0;1;0;1;0;1;0]; zeros(20,1); ones(20,1); zeros(5,1)];
%     connected = ones(length(condition),1); 
    connected = [zeros(15,1); ones(18,1)]; 
    connectionStiffness = connected*Ks;
    connectionDamping = connected*Ds;
end

% target position and velocity variance (per trial in the columns, in
% meter]
trialPosSD = 0.004*ones(2,size(connected,1));   % [green subject ; blue subject] 

% % VelSD for VN experiment 
% VelSD_tmp = repmat([linspace(0.005,0.1,10) 0.15 0.225 0.3],1,4); 
% VelSD_sort = randperm(length(VelSD_tmp)); 
% trialVelSD = repmat([zeros(1,8) VelSD_tmp(VelSD_sort)],2,1);

% VelSD for ffemg experiment 
%trialVelSD = zeros(2,size(connected,1));   

VelSD_tmp = repmat([0.005 0.029 0.053 0.076 0.10 0.005 0.005 0.005 0.005;  0.005 0.005 0.005 0.005 0.005  0.029 0.053 0.076 0.10],1,2);
VelSD_sort = randperm(length(VelSD_tmp)); 
trialVelSD = [0.005*ones(2,15) VelSD_tmp(:,VelSD_sort)];   

%% randomization
% if exist('exp_ffemg_protocols/exp_ffemg_randomization.mat','file')
%     load('exp_ffemg_protocols/exp_ffemg_randomization.mat');
% else 
%     trialRandomization = 30*rand(size(condition));
%     save('exp_ffemg_protocols/exp_ffemg_randomization.mat','trialRandomization');
% end


if exist('exp_test_protocols/exp_test_randomization.mat','file')
    load('exp_test_protocols/exp_test_randomization.mat');
else
    trialRandomization = 20*rand(size(condition));
    save('exp_test_protocols/exp_test_randomization.mat','trialRandomization');
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
    % target position standard deviation
    trial{ii}.targetPosSD.id0 = trialPosSD(1,ii);
    trial{ii}.targetPosSD.id1 = trialPosSD(2,ii);
    
    % target velocity standard deviation
    trial{ii}.targetVelSD.id0 = trialVelSD(1,ii);
    trial{ii}.targetVelSD.id1 = trialVelSD(2,ii);
end

%% block data
% indicate how the trialst are divided over the blocks
% NOTE: you alwayst need at leastt 1 block
numBlockst = length(divTrialsOverBlocks);
for ii = 1:numBlockst
    st.experiment.block{ii}.breakDuration = 20.0;
    st.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrialsOverBlocks{ii})
        st.experiment.block{ii}.trial{jj} = trial{divTrialsOverBlocks{ii}(jj)};
    end
    
    expprotocol.block(ii).connected = connected(divTrialsOverBlocks{ii});
    expprotocol.block(ii).condition = condition(divTrialsOverBlocks{ii});
    expprotocol.block(ii).trialRandomization = trialRandomization(divTrialsOverBlocks{ii});
    expprotocol.block(ii).connectionStiffness = connectionStiffness(divTrialsOverBlocks{ii});
    expprotocol.block(ii).connectionDamping = connectionDamping(divTrialsOverBlocks{ii});
    expprotocol.block(ii).velocitySDid0 = trialVelSD(1,divTrialsOverBlocks{ii});
	expprotocol.block(ii).velocitySDid1 = trialVelSD(2,divTrialsOverBlocks{ii});

end



%% stave everything (in xml and mat)

if ~exist(protocolpath,'dir')
    mkdir(protocolpath);
end

% check if you want to overwrite the protocol
saveProtocol = 1;
if exist([protocolpath filesep filename '.xml'],'file') || exist([protocolpath filesep filename],'file')
    promptMeststage = sprintf('Thist file already exists:\n%st\nDo you want to overwrite it?', [protocolpath filesep filename '.xml']);
	titleBarCaption = 'Overwrite protocol?';
	buttonText = questdlg(promptMeststage, titleBarCaption, 'Yes', 'No', 'No');
    
    if strcmpi(buttonText,'No')
        saveProtocol = 0;
    end
end

if saveProtocol
    % write to to XML file
    struct2xml(st,[protocolpath filesep filename '.xml']);
    % stave experiment sttruct in mat file
    save([protocolpath filesep filename], 'st','expprotocol');
end





