function data = importTCdata(datapath, modelname)
%% importTCdata
%
% Function to load twincat data.
% Prerequisite: sort_nat (). Put this function in your matlab main folder
% (Documents/MATLAB).
%
% Niek Beckers

%% check input
% no data path given? Call UI to select folder manually
if nargin < 1
    datapath = uigetdir;
end

%% load measurement data
% currentdir = [fileparts(mfilename('fullpath')) filesep];
currentdir = pwd;

cd(datapath);                                   % go to data directory

datafiles = dir('data_part*.mat');              % get list of all datafiles
filenames = sort_nat({datafiles(:).name});      % sort files (sort_nat needed)

dataArray = [];
for ii = 1:length(filenames)
    name = [filenames{ii}];
    load(name);
    paramname = strrep(strrep(name,'part',''),'.mat','');
    try
        eval(['data_temp = ' paramname ';']);
    catch
        c = who('data_*');
            eval(['data_temp = ' c{1} ';']);

    end
    dataArray = [dataArray data_temp];
    clear data_*
end

cd(currentdir); % go back to current directory
dataArray = dataArray';       % to columns


%% parameter names/labels
% based on which model, make parameter labels and indices
if (nargin < 2)
    modelname = 'model_base_bros';
end

str_param_def = {'SystemState';'qJointSpace.q_AbsEnc';'qJointSpace.qdot_AbsEnc';'qJointSpace.q_IncEnc';'qJointSpace.qdot_IncEnc';'xOpSpace.x_AbsEnc';'xOpSpace.xdot_AbsEnc';'xOpSpace.x';'xOpSpace.xdot';'Controller_ToLogger.x_ref';'Controller_ToLogger.xdot_ref';'Controller_ToLogger.xddot_ref';'qJointSpace.JointTorque_Nm';'BusFTSensor.ForcesOpSpace'};

switch lower(modelname)
   
    case 'model_base_bros'
        %% model_base_bros
        param_lbls = [  'time';
                        strcat(str_param_def,'_BROS1');
                        strcat(str_param_def,'_BROS2');
                        'target_BROS1';
                        'target_BROS2';
                        'cursor_BROS1';
                        'cursor_BROS2';
                        'Error_BROS1';
                        'Error_BROS2';
                        'ExpTrialNumber';
                        'ExpTrialRunning';
                        'Admittance_Mv';
                        'Admittance_Bv'];
                    
        % indices corresponding to each parameter
        param_idx = {1;... % time 
                     2;  3:4;   5:6;   7:8;   9:10;  11:12; 13:14; 15:16; 17:18; 19:20; 21:22; 23:24; 25:26; 27:32;... % BROS1
                     33; 34:35; 36:37; 38:39; 40:41; 42:43; 44:45; 46:47; 48:49; 50:51; 52:53; 54:55; 56:57; 58:63;... % BROS2
                     64:65; 66:67; 68:69; 70:71; 72; 73;... % target, cursor, error
                     74; 75;... % experiment trial
                     76; 77; }; % admittance
        
        % add more admittance data  ("freshly added", so append to param_idx and param_lbls)
        if (size(dataArray,2) > 77)
            param_lbls = [param_lbls; 'isConnected'; 'Kp_s'; 'Kd_s'; 'Fs1'; 'Fs2'];
            param_idx = [param_idx; 78; 79; 80;81:82; 83:84 ]; 
        end

        % I also added the target velocity to the model, so append to param_idx and param_lbls
        if (size(dataArray,2) > 84)
            param_lbls = [param_lbls; 'target_vel_BROS1'; 'target_vel_BROS2'];
            param_idx = [param_idx; 85:86; 87:88 ]; %#ok<NASGU>
        end
        

    case 'model_sysid'
        %% model_sysid
        param_lbls = [  'time';
                        strcat(str_param_def,'_BROS1');
                        'Error_BROS1';
                        'ExpTrialNumber';
                        'ExpTrialRunning'];
        param_idx = {1;... % time 
                     2;  3:4;   5:6;   7:8;   9:10;  11:12; 13:14; 15:16; 17:18; 19:20; 21:22; 23:24; 25:26; 27:32;... % BROS1
                     33; %error
                     34; 35;... % experiment trial
                     };  %#ok<NASGU>
    case 'model_affc'
        %% model_affc
        param_lbls = [  'time';
                        strcat(str_param_def,'_BROS1');
                        'Controller_ToLogger.tau_y';
                        'Controller_ToLogger.tau_affc';
                        'Controller_ToLogger.p';
                        'Error_BROS1';
                        'ExpTrialNumber';
                        'ExpTrialRunning'];
        param_idx = {1;... % time 
                     2;  3:4;   5:6;   7:8;   9:10;  11:12; 13:14; 15:16; 17:18; 19:20; 21:22; 23:24; 25:26; 27:32;... % BROS1
                     33:34; 35:36; % tau_y, tau_affc
                     37:43; % p
                     44; %error
                     45; 46;... % experiment trial
                     };  %#ok<NASGU>
end

 

% create struct with all data parameters
data = struct;
for ii = 1:length(param_lbls)
    param = param_lbls{ii};
    if ~isempty(strfind(param,'.')) 
        if strcmpi(version('-release'),'2016b')
            param = extractAfter(param,'.'); 
        else
            ix = strfind(param,'.');
            param = param(ix(end)+1:end);
        end
    end
    
    eval(['data.' char(param) ' = dataArray(:,param_idx{ii});']);
end



%  str_param_BROS1 = {'SystemState_BROS1';'qJointSpace.q_AbsEnc_BROS1';'qJointSpace.qdot_AbsEnc_BROS1';'qJointSpace.q_IncEnc_BROS1';'qJointSpace.qdot_IncEnc_BROS1';'xOpSpace.x_AbsEnc_BROS1';'xOpSpace.xdot_AbsEnc_BROS1';'xOpSpace.x_BROS1';'xOpSpace.xdot_BROS1';'Controller_ToLogger.x_ref_BROS1';'Controller_ToLogger.xdot_ref_BROS1';'Controller_ToLogger.xddot_ref_BROS1';'qJointSpace.JointTorque_Nm_BROS1';'BusFTSensor.ForcesOpSpace_BROS1'};
%  str_param_BROS2 = {'SystemState_BROS2';'qJointSpace.q_AbsEnc_BROS2';'qJointSpace.qdot_AbsEnc_BROS2';'qJointSpace.q_IncEnc_BROS2';'qJointSpace.qdot_IncEnc_BROS2';'xOpSpace.x_AbsEnc_BROS2';'xOpSpace.xdot_AbsEnc_BROS2';'xOpSpace.x_BROS1';'xOpSpace.xdot_BROS1';'Controller_ToLogger.x_ref_BROS2';'Controller_ToLogger.xdot_ref_BROS2';'Controller_ToLogger.xddot_ref_BROS2';'qJointSpace.JointTorque_Nm_BROS2';'BusFTSensor.ForcesOpSpace_BROS2'};

%     % model name is given, load variable names from model
%     load_system(get_param(modelname,'filename'));
%     str_param_BROS1 = strsplit(get_param([modelname '/BusSelectorLoggerBROS1'],'OutputSignals'),',').';
%     str_param_BROS1 = strcat(str_param_BROS1,'_BROS1');
%     str_param_BROS2 = strsplit(get_param([modelname '/BusSelectorLoggerBROS2'],'OutputSignals'),',').';
%     str_param_BROS2 = strcat(str_param_BROS2,'_BROS2');

% these are the parameter labels in the recorded data
% param_lbls = ['time';
%     str_param_BROS1;
%     str_param_BROS2;
%     'target_BROS1';
%     'target_BROS2';
%     'cursor_BROS1';
%     'cursor_BROS2';
%     'Error_BROS1';
%     'Error_BROS2';
%     'ExpTrialNumber';
%     'ExpTrialRunning';
%     'Admittance_Mv';
%     'Admittance_Bv'];   