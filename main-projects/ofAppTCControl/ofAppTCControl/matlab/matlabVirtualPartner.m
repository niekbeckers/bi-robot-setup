function matlabVirtualPartner
callerID = '[MATLABVIRTUALPARTNER]: ';
disp([callerID 'Starting up ' mfilename]);

%% initialize
cntr_filename = 0;
datapath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\experiments\virtual-agent\data\';
exepath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\main-projects\ofAppTCControl\ofAppTCControl\matlab\';
filepath = 'vpFitSettings_trial';
loopPause = 0.5;

% parpool
if (size(gcp) == 0)
    parpool(2,'IdleTimeout',30); % setup workers with idle timeout of 30 minutes
end

%% while loop
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
        out.VP = struct;
        
        % load data of trial with trialID
        data = loadTrialData(fitIDs,trialID,datapath);
            
        % perform optimization
        parfor ii = 1:length(fitIDs)
%             datasel.target = data.(['target_BROS' num2str(fitIDs(ii))]);
            % perform model fit
            %[out.x] = doModelFit(datasel);
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
        else
            % model fit threw error
            switch(errorFlag)
                case 1
                    disp('Model fit returned error flag 1: system is unstable');
                case 2
                    disp('Model fit returned error flag 2: fit is too inaccurate');
            end
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
for ii = length(filenames)-nrTrialsLookback+1:length(filenames)
    if (ii < 0), continue; end % in case less then nrTrialsLookback are present
    
    name = [filenames{ii}];
    load(name);
    paramname = strrep(strrep(name,'part',''),'.mat','');
    eval(['data_temp = ' paramname ';']);
    dataArray = [dataArray data_temp];
    clear data_*
end

cd(currentdir); % go back to current directory
dataArray = dataArray';       % to columns

param_lbls = ['time';
    'xOpSpace.xdot_BROS1';
    'xOpSpace.xdot_BROS2';
    'target_BROS1';
    'target_BROS2';
    'cursor_BROS1';
    'cursor_BROS2';
    'target_vel_BROS1'; 
    'target_vel_BROS2';
    'ExpTrialNumber';
    'ExpTrialRunning'];   

% indices corresponding to each parameter
param_idx = {1;... % time 
             17:18; 48:49; % xdot
             64:65; 66:67; 68:69; 70:71; 85:86; 87:88;... % target, cursor, vel_target
             74; 75}; % experiment trial


% create struct with all data parameters
dataraw = struct;
for ii = 1:length(param_lbls)
    param = param_lbls{ii};
    if ~isempty(strfind(param,'.')) 
        param = extractAfter(param,'.'); 
    end
    eval(['dataraw.' char(param) ' = dataArray(:,param_idx{ii});']);
end

% extract trials
idxtrial = findseq(double(dataraw.ExpTrialNumber == trialID & dataraw.ExpTrialRunning));
idx = idxtrial(1,2):idxtrial(1,3);

data = struct;
% make own time vector per trial
t = dataraw.time(idx); t = t - t(1);
data.t = t;

% retrieve dt (assume it's a multiple of 1ms)
dt = 0.001;
dt = round(mode(diff(t))/dt)*dt;

% resample data
vars = {};
for ii = 1:length(brosIDs)
    vars{end+1} = ['target_BROS' num2str(brosIDs(ii))];
    vars{end+1} = ['cursor_BROS' num2str(brosIDs(ii))];
    vars{end+1} = ['vel_target_BROS' num2str(brosIDs(ii))];
end

% select the data per trial
for jj = 1:length(vars)
     [tres,datares]= resampleTCdata(t,dataraw.(vars{jj})(idx,:),dt);
     data.(vars{jj}) = datares;
     data.t = tres;
end

end