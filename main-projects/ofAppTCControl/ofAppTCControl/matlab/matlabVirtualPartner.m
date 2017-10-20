function matlabVirtualPartner
%% function matlabVirtualPartner

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
        clear data
        data = loadTrialData(fitIDs,trialID,datapath);
            
        
        % select data for optim function
        dataArray = NaN(length(data.t(1:10:end)),8,length(fitIDs));
        for ii = 1:length(fitIDs)
            id = fitIDs(ii);
            x = data.(['cursor_BROS' num2str(id)])(1:10:end,:);
            xdot = data.(['xdot_BROS' num2str(id)])(1:10:end,:);
            target = data.(['target_BROS' num2str(id)])(1:10:end,:);
            target_val = data.(['target_vel_BROS' num2str(id)])(1:10:end,:);
            dataArray(:,:,ii) = [x xdot target target_val];
        end

        % perform optimization
        datamodelfit = struct;
        % define number of tasks (for parfor loop)
        nrTasks = length(fitIDs);
        
        parfor ii = 1:nrTasks
%             datatmp = dataArray(:,:,ii);
            % perform model fit
            %[out.x] = doModelFit(datatmp);
        end
        
%         % DEBUG dummy output
%         out.VP.trialID = s.VP.trialID;
%         out.VP.executeVirtualPartner.id0 = 0;
%         out.VP.executeVirtualPartner.id1 = 0;
%         out.VP.error.id0 = 0;
%         out.VP.error.id1 = 0;
%         out.VP.modelparameters.bros1.x1 = randn(1);
%         out.VP.modelparameters.bros1.x2 = randn(1);
%         out.VP.modelparameters.bros1.x3 = randn(1);
%         out.VP.modelparameters.bros2.x1 = randn(1);
%         out.VP.modelparameters.bros2.x2 = randn(1);
%         out.VP.modelparameters.bros2.x3 = randn(1);

        
        if (errorFlag == 0)
            % write results to XML file (and store mat file)
            outputfile = [exepath 'fitResults_trial' num2str(datamodelfit.VP.trialID)];
            save(outputfile,'datamodelfit');
            writeXML(datamodelfit,[outputfile '.xml']);
            
            disp([callerID 'Results written to ''fitResults_trial' num2str(datamodelfit.VP.trialID) '.xml/mat''']);
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
%% function [loadOkay,s] = readXML(filename)

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
    
    % parse xml file
    if isfield(xml.VP, 'trialID')
        s.VP.trialID = str2double(xml.VP.trialID.Text);
    else
        s.VP.trialID = [];
        loadOkay = false;
    end
    
    % doFitForBROS
    if isfield(xml.VP, 'doFitForBROSID')
        flds = fieldnames(xml.VP.doFitForBROSID);
        for ii = 1:length(flds)
            s.VP.doFitForBROSID(ii) = str2double(xml.VP.doFitForBROSID.(flds{ii}).Text);
        end
    else
        s.VP.doFitForBROSID = [];
        loadOkay = false;
    end
    
    % check if we need to use a define x0 for the model fit
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
%% function [writeOkay] = writeXML(s, filename)
try 
    struct2xml(s,filename);
    writeOkay = true;
catch
    writeOkay = false;
end

end

function [data] = loadTrialData(datapath,trialID)
%% function data = loadTrialData(datapath,trialID)

nrTrialsLookback = 5; % select last 5 data files,
currentdir = pwd;
cd(datapath);                                   % go to data directory
datafiles = dir('data_part*.mat');              % get list of all datafiles
filenames = sort_nat({datafiles(:).name});      % sort files (sort_nat needed)
ixstart = length(filenames)-nrTrialsLookback+1; if (ixstart < 1), ixstart = 1; end
ixend = length(filenames);
idxcopy = ixstart:ixend; % files to copy

tmpDir = 'tmpDirDataModelFit';
% copy files to folder
if ~exist(tmpDir,'dir')
    mkdir(tmpDir); 
else
    % clear any data files from this directory
    delete(fullfile(cd, [tmpDir filesep 'data_part*.mat']));
end
% copy the new data files to the tmpDir
cellfun(@(x)copyfile(x,tmpDir), filenames(idxcopy));

% load all data
alldata = importTCdata(tmpDir,'model_base_bros');

cd(currentdir);

% extract trials
idxtrial = findseq(double(alldata.ExpTrialNumber == trialID & alldata.ExpTrialRunning));
idx = idxtrial(1,2):idxtrial(1,3);

data = struct;
params = fieldnames(alldata);

for ii = 1:length(params)
    data.(params{ii}) = alldata.(params{ii})(idx,:);
end

% add time vector
t = alldata.time(idx); t = t - t(1);
data.t = t;

end