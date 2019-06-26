%% createBROstExperimentProtocol_template
% Niek Beckerst
% May 2017

clear variables; close all; clc;

% Change before running protocol 
type = 'pilot';          % pilot or experiment 
pairNr = 1;

expID = 'CI';
groupType = 'interaction'; % solo or interaction
Kdes = 100; 
Ks = 1/((1/Kdes)-(1/1500));
Ds = 3;
MVC = 8; 

% filename
protocolpath = 'exp_CI_protocols';
filename = [type filesep 'pair' num2str(pairNr) filesep 'protocol_' expID '_pair' num2str(pairNr)];

% create (main) sttruct
st = struct;

% indicate which type of trial feedback
st.experiment.expID = expID;
st.experiment.trialFeedback = 1;
st.experiment.sessionNr = 1;
st.experiment.partnersNr = pairNr;

% st.experiment.doVirtualPartner = 0;
st.experiment.activeBROSID.id0 = 1;
st.experiment.activeBROSID.id1 = 2;

st.experiment.displayType = 0;    % 0 = pursuit_2D; 1 = pursuit_1D; 2 = compensatory_1D; 3 = compensatory_2D; 4 = pursuit_roll; 5 = compensatory_roll 
st.experiment.cursorShape = 3;    % 0 = circle; 1 = circle_open; 2 = line; 3 = cross; 4 = crosshair   
st.experiment.targetMode = 2;     % 0 = normal; 1 = cloud; 2 = moving cloud; 3 = exploding cloud 

% target stettings
st.experiment.targetParticleCount = 5;
st.experiment.targetParticlePeriod = 0.5;   % how long each child particle mopves before reset
st.experiment.targetParticleSize = 6.0;     % size of partticle (px)

%% trial data

% trial stettings

% experiment stettings

% condition vector: each index is a trial
% condition to -1: baseline trial (no movement)

condition = [-1*ones(MVC+1,1); zeros(85,1)]; 

divTrialsOverBlocks = {1:1+MVC; MVC+2:MVC+21; MVC+22:MVC+36; MVC+37:MVC+51; MVC+52:MVC+66; MVC+67:MVC+86};

numTrials = numel(condition); % example
breakDuration = 0*ones(numTrials,1);
trialDuration = [20; 30*ones(MVC,1); 23*ones(numTrials-(MVC+1),1)];

% connection
if strcmpi(groupType,'solo')
    connected = zeros(numTrials,1);
    connectionstiffness = zeros(numTrials,1);
    connectionDamping = zeros(numTrials,1);
elseif strcmpi(groupType,'interaction')
    exptr_15 = [zeros(5,1); ones(10,1)]; 
    exptr_20 = [zeros(5,1); ones(15,1)]; 
    connected = [zeros(MVC+1,1); zeros(20,1); exptr_15(randperm(length(exptr_15))); exptr_15(randperm(length(exptr_15))); exptr_15(randperm(length(exptr_15))); exptr_20(randperm(length(exptr_20)))];
    connectionstiffness = connected*Ks;
    connectionDamping = connected*Ds;
end

% target position and velocity variance (per trial in the columns, in
% meter]
trialPosSD = 0.004*ones(2,size(connected,1));   % [green subject ; blue subject] 


VelSD = linspace(0.005,0.10,5); % SD of velocity 
VelSD_conn = [repmat(VelSD,1,5) 0.005*ones(1,20); 0.005*ones(1,25) repmat(VelSD(2:end),1,5)]; % 9 possible combinations of VelSD, repeated 5 times 
VelSD_conn_rand = VelSD_conn(:,randperm(length(VelSD_conn))); % VelSD_conn randomized 
VelSD_sing_rand = [VelSD(randperm(length(VelSD))) VelSD(randperm(length(VelSD))) VelSD(randperm(length(VelSD))) VelSD(randperm(length(VelSD)));...
                  VelSD(randperm(length(VelSD))) VelSD(randperm(length(VelSD))) VelSD(randperm(length(VelSD))) VelSD(randperm(length(VelSD)))]; % randomize the order of single trials within blocks  
VelSD_training  = [VelSD(:,randperm(length(VelSD))); VelSD(:,randperm(length(VelSD)))]; 
              
if strcmpi(groupType,'solo')
    trialVelSD = 0.005*ones(2,length(connected)); 
elseif strcmpi(groupType,'interaction')              
    trialVelSD(:,connected(MVC+22:end)==1)=VelSD_conn_rand; % connected trials 
    trialVelSD(:,connected(MVC+22:end)==0)=VelSD_sing_rand; % single trials 
    trialVelSD = [0.005*ones(2,MVC+11) repmat(VelSD,2,1) VelSD_training trialVelSD]; 
end 


%% randomization
if exist('exp_CI_protocols/exp_CI_randomization.mat','file')
    load('exp_CI_protocols/exp_CI_randomization.mat');
else 
    trialRandomization = 30*rand(size(condition));
    save('exp_CI_protocols/exp_CI_randomization.mat','trialRandomization');
end

%% prepare trials
for ii = 1:numTrials
    trial{ii}.connected = connected(ii);
    trial{ii}.connectionstiffness = connectionstiffness(ii);
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
    st.experiment.block{ii}.breakDuration = 240.0;
    st.experiment.block{ii}.homingType = 302;
    for jj = 1:length(divTrialsOverBlocks{ii})
        st.experiment.block{ii}.trial{jj} = trial{divTrialsOverBlocks{ii}(jj)};
    end
    
    expprotocol.block(ii).connected = connected(divTrialsOverBlocks{ii});
    expprotocol.block(ii).condition = condition(divTrialsOverBlocks{ii});
    expprotocol.block(ii).trialRandomization = trialRandomization(divTrialsOverBlocks{ii});
    expprotocol.block(ii).connectionstiffness = connectionstiffness(divTrialsOverBlocks{ii});
    expprotocol.block(ii).connectionDamping = connectionDamping(divTrialsOverBlocks{ii});
    expprotocol.block(ii).velocitySDid0 = trialVelSD(1,divTrialsOverBlocks{ii});
	expprotocol.block(ii).velocitySDid1 = trialVelSD(2,divTrialsOverBlocks{ii});

end



%% save everything (in xml and mat)

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





