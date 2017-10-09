function matlabVirtualPartner
callerID = '[MATLABVIRTUALPARTNER]: ';
disp([callerID 'Starting up ' mfilename]);

%% initialize
cntr_filename = 0;
datapath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\main-projects\data\bros\';
exepath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\main-projects\ofAppTCControl\ofAppTCControl\matlab\';
filepath = 'vpFitSettings_trial';
loopPause = 0.5;

% parpool
if (size(gcp) == 0)
    parpool(2,'IdleTimeout',30); % setup workers with idle timeout of 30 minutes
end

keepRunning = true;
while (keepRunning)
    try
        [loadOkay,s] = readXML([exepath filepath num2str(cntr_filename) '.xml']);
    catch
        loadOkay = false;
    end
    
    if loadOkay
        if isfield(s.VP, 'terminate')
            keepRunning = false;
            continue; % go to next iteration of while loop, skip everything below
        end
        errorFlag = 0;
        
        disp([callerID 'Loaded ' filepath num2str(cntr_filename) ', starting model fit']);
        
        % initialize and prepare stuff
        fitIDs = s.VP.doFitForBROSID;
        trialID = s.VP.trialID;
        numFits = length(fitIDs);
        out.VP = struct;
        
        % load data of trial with trialID
        data = loadTrialData(fitIDs,trialID,datapath);
            
        % perform optimization
        parfor ii = 1:numFits
            % perform model fit
            
        end
        
        % DEBUG dummy output
        out.VP.trialID = s.VP.trialID;
        out.VP.executeVirtualPartner.id0 = 0;
        out.VP.executeVirtualPartner.id1 = 0;
        out.VP.error.id0 = 0;
        out.VP.error.id1 = 0;
        out.VP.modelparameters.bros1.x1 = randn(1);
        out.VP.modelparameters.bros1.x2 = randn(1);
        out.VP.modelparameters.bros1.x3 = randn(1);
        out.VP.modelparameters.bros2.x1 = randn(1);
        out.VP.modelparameters.bros2.x2 = randn(1);
        out.VP.modelparameters.bros2.x3 = randn(1);

        % write results to XML file
        
        if (errorFlag == 0)
            outputfile = [exepath 'fitResults_trial' num2str(out.VP.trialID) '.xml'];
            writeXML(out,outputfile);
            disp([callerID 'Results written to ''fitResults_trial' num2str(out.VP.trialID) '.xml''']);
        end
        cntr_filename = cntr_filename+1;
    end
    pause(loopPause);
end

end

function [loadOkay,s] = readXML(filename)

loadOkay = false;
s = struct;

if exist(filename,'file')
    try
        xml = xml2struct(filename);
        loadOkay = true;
    catch
        loadOkay = false;
        return;
    end
    
    % check if an external terminate request is made.
    if isfield(xml.VP,'terminate')
        s.VP.terminate = true;
        return
    end
    
    % parse
    if isfield(xml.VP, 'trialID')
        s.VP.trialID = str2double(xml.VP.trialID.Text);
    else
        s.VP.trialID = [];
        loadOkay = false;
    end
    
    if isfield(xml.VP, 'doFitForBROSID')
        flds = fieldnames(xml.VP.doFitForBROSID);
        for ii = 1:length(flds)
            s.VP.doFitForBROSID(ii) = str2double(xml.VP.doFitForBROSID.(flds{ii}).Text);
        end
    else
        s.VP.doFitForBROSID = [];
        loadOkay = false;
    end
    
    if isfield(xml.VP,'useX0')
        flds = fieldnames(xml.VP.useX0);
        for ii = 1:length(flds)
            s.VP.useX0(ii) = str2double(xml.VP.useX0.(flds{ii}).Text);
        end
    else
        xml.VP.useX0 = [];
    end
    
end
end

function [writeOkay] = writeXML(s, filename)

try 
    struct2xml(s,filename);
catch
    writeOkay = false;
end

end

function data = loadTrialData(brosIDs,trialID,datapath)


currentdir = pwd;
cd(datapath);                                   % go to data directory
datafiles = dir('data_part*.mat');              % get list of all datafiles
filenames = sort_nat({datafiles(:).name});      % sort files (sort_nat needed)

dataArray = [];
nrTrialsLookback = 5; % select last 5 data files,
for ii = length(filenames)-nrTrialsLookback:length(filenames)
    if (ii < 0), continue; end
    name = [filenames{ii}];
    load(name);
    paramname = strrep(strrep(name,'part',''),'.mat','');
    eval(['data_temp = ' paramname ';']);
    dataArray = [dataArray data_temp];
    clear data_*
end

cd(currentdir); % go back to current directory
dataArray = dataArray';       % to columns

str_param_BROS1 = {'SystemState_BROS1';'qJointSpace.q_AbsEnc_BROS1';'qJointSpace.qdot_AbsEnc_BROS1';'qJointSpace.q_IncEnc_BROS1';'qJointSpace.qdot_IncEnc_BROS1';'xOpSpace.x_AbsEnc_BROS1';'xOpSpace.xdot_AbsEnc_BROS1';'xOpSpace.x_BROS1';'xOpSpace.xdot_BROS1';'Controller_ToLogger.x_ref_BROS1';'Controller_ToLogger.xdot_ref_BROS1';'Controller_ToLogger.xddot_ref_BROS1';'qJointSpace.JointTorque_Nm_BROS1';'BusFTSensor.ForcesOpSpace_BROS1'};
str_param_BROS2 = {'SystemState_BROS2';'qJointSpace.q_AbsEnc_BROS2';'qJointSpace.qdot_AbsEnc_BROS2';'qJointSpace.q_IncEnc_BROS2';'qJointSpace.qdot_IncEnc_BROS2';'xOpSpace.x_AbsEnc_BROS2';'xOpSpace.xdot_AbsEnc_BROS2';'xOpSpace.x_BROS1';'xOpSpace.xdot_BROS1';'Controller_ToLogger.x_ref_BROS2';'Controller_ToLogger.xdot_ref_BROS2';'Controller_ToLogger.xddot_ref_BROS2';'qJointSpace.JointTorque_Nm_BROS2';'BusFTSensor.ForcesOpSpace_BROS2'};

param_lbls = ['time';
    str_param_BROS1;
    str_param_BROS2;
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


% create struct with all data parameters
dataraw = struct;
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
    
    eval(['dataraw.' char(param) ' = dataArray(:,param_idx{ii});']);
end


idxtrial = findseq(double(dataraw.ExpTrialNumber == trialID & dataraw.ExpTrialRunning));
idx = idxtrial(1,2):idxtrial(1,3);

data = struct;
% make own time vector per trial
t = dataraw.time(idx); t = t - t(1);
data.trial(ii).t = t;

% retrieve dt (assume it's a multiple of 1ms)
dt = 0.001;
dt = round(mode(diff(t))/dt)*dt;

% get the variable names. Remove 'BusActuator1', 'BusActuator2' and 't'
vars = {};
for ii = 1:length(brosIDs)
    vars{end+1} = ['target_BROS' num2str(brosIDs(ii))];
    vars{end+1} = ['cursor_BROS' num2str(brosIDs(ii))];
end

% select the data per trial
for jj = 1:length(vars)
     [tres,datares]= resampleTCdata(t,dataraw.(vars{jj})(idx,:),dt);
     data.trial(ii).(vars{jj}) = datares;
     data.trial(ii).t = tres;
end

end