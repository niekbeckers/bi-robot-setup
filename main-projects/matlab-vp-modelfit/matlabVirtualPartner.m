function matlabVirtualPartner
%% function matlabVirtualPartner


cd(fileparts(mfilename('fullpath')));
addpath('scripts');

callerID = '[MATLABVIRTUALPARTNER]: ';
disp([callerID 'Starting up ' mfilename]);

%% initialize/setup
cntr_filename = 0;
datapath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\experiments\virtual-agent\data\';
settingspath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\main-projects\matlab-vp-modelfit\settings\';
resultspath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\main-projects\matlab-vp-modelfit\results\';
settings_filename = 'settings_vpmodelfit_trial';
loopPause = 0.5;

% create folder for model output files (for copies)
pathoutputstore = ['results-modelfit-' datestr(now,'ddmmyy-HHMM')];
if ~exist(pathoutputstore,'dir')
    mkdir([resultspath pathoutputstore]);
end

% parpool
if (size(gcp) == 0)
    parpool(2,'IdleTimeout',30); % setup workers with idle timeout of 30 minutes
end

% preallocate 'saved p0'
p0_saved = [];

%% while loop
disp([callerID 'Running ' mfilename]);
keepRunning = true;
while (keepRunning)
    try
        [loadOkay,s] = readXML([settingspath settings_filename num2str(cntr_filename) '.xml']);
    catch
        loadOkay = false;
    end
    
    if loadOkay
        if isfield(s.VP, 'terminate')
            keepRunning = false;
            continue; % go to next iteration of while loop, skip everything below
        end
        errorFlag = 0;
        
        disp([callerID 'Loaded ' settings_filename num2str(cntr_filename) ', starting model fit']);
        
        % initialize and prepare stuff
        try
        fitIDs = s.VP.doFitForBROSID(:);
        trialID = s.VP.trialID;
        condition = s.VP.condition;
        catch me
            disp(me)
            keyboard
        end
        % load data of trial with trialID
        clear data
        data = loadTrialData(datapath,trialID);
        
        % select data for optim function
        ds = 20;
        dataArray = NaN(length(data.t(1:ds:end)),8,length(fitIDs));
        t = data.t(1:ds:end,:);
        dt = mode(diff(t));
        for ii = 1:length(fitIDs)
            id = fitIDs(ii);
            
            x = data.(['cursor_BROS' num2str(id)])(1:ds:end,:);
            xdot = data.(['xdot_BROS' num2str(id)])(1:ds:end,:);
            target = data.(['target_BROS' num2str(id)])(1:ds:end,:);
            target_vel = data.(['target_vel_BROS' num2str(id)])(1:ds:end,:);
            dataArray(:,:,ii) = [x xdot target target_vel];
        end
        

        % perform model fit
        resultsmodelfit.VP = struct;
        resultsmodelfit.VP.trialID = trialID;
        resultsmodelfit.VP.doFitForBROSID = fitIDs;
        resultsmodelfit.VP.dt = dt;
        
        % define number of tasks (for parfor loop)
        nrP0 = 3; % number of initial parameter estimates
        nrFitParams = 3;
        idxIDs = reshape(repmat(fitIDs,1,nrP0).',[],1); % vector with fitIDs
        p0 = NaN(nrFitParams,nrP0);
        
        % upper and lower bounds
        ub = [1e4;1e2;1e-2];
        lb = [0;0;0];
        
        % create p0's
        for ii = 1:numel(idxIDs)
            % either use previous fit values (per fitID) or generate a
            % random p0.
            id = idxIDs(ii);
            if ~isempty(p0_saved)
                p0(:,ii) = normrnd(p0_saved(:,id), (ub-lb)/100);
                p0(:,ii) = min(max(p0(:,ii),lb),ub); % bound p0 by upper and lower bound
%                 p0(:,ii) = [normrnd(p0_saved(1,id),ub(1)/12); normrnd(p0_saved(2,id),ub(2)/12); normrnd(p0_saved(3,id),ub(3)/12)];
            else 
                p0(:,ii) = lb+rand(3,1).*(ub-lb); % [rand*ub(1); rand*ub(2); rand*ub(3)];  
                p0(:,ii) = min(max(p0(:,ii),lb),ub);
            end
        end 

        % perform model fits
        nrTasks = length(fitIDs)*nrP0;
        pfit_all = NaN(nrFitParams,nrTasks);
        errorFlagfit = zeros(1,nrTasks);
        parfor it = 1:nrTasks
            % perform model fit
            [pfit_all(:,it), fvalfit(it), fitInfo(it), errorFlagfit(it)] = doModelFit(dataArray(:,:,idxIDs(it)),dt,p0(:,it),condition);
        end
        
        % store all iterations
        resultsmodelfit.VP.iterations.p0 = p0;
        resultsmodelfit.VP.iterations.idxIDs = idxIDs;
        resultsmodelfit.VP.iterations.fitInfo = fitInfo;
        resultsmodelfit.VP.iterations.fvalfit = fvalfit;
        resultsmodelfit.VP.iterations.errorFlag = errorFlagfit;
        
        try
        % select the best fit per fitID
        pfit_opt = NaN(nrFitParams,length(fitIDs));
        for ii = 1:length(fitIDs)
            id = fitIDs(ii);
            idx = idxIDs == id;
            
            % select minimum fval solution
            [~,I] = min(fvalfit(idx));
            
            % errors etc
            resultsmodelfit.VP.error.(['id' num2str(id-1)]) = errorFlagfit(I);
            if errorFlagfit(I) == 0
                resultsmodelfit.VP.executeVirtualPartner.(['id' num2str(id-1)]) = 1;
            else
                resultsmodelfit.VP.executeVirtualPartner.(['id' num2str(id-1)]) = 0;
            end
            
            % model parameters
            pfit_opt(:,ii) = pfit_all(:,I);
            for jj = 1:size(pfit_opt,1)
                resultsmodelfit.VP.modelparameters.(['bros' num2str(id)]).(['x' num2str(jj)]) = pfit_opt(jj,ii);
            end
        end
        catch me
            disp(me)
            keyboard
        end
       
        % store data in mat file (regardless of fiterror)
        outputfile = [resultspath 'results_vpmodelfit_trial' num2str(resultsmodelfit.VP.trialID)];
        save(outputfile,'resultsmodelfit','dataArray'); % save to mat files
        copyfile([outputfile '.mat'],pathoutputstore); % copy to output file store
        
        if (errorFlag == 0)
            % write results to XML file (and store mat file)
            writeXML(resultsmodelfit,[outputfile '.xml']); 
            
            disp([callerID 'Results written to ''settings_vpmodelfit_trial' num2str(resultsmodelfit.VP.trialID) '.xml/mat''']);
        else
            % model fit threw error
            switch(errorFlag)
                case 1
                    disp('Model fit returned error flag 1: system is unstable');
                case 2
                    disp('Model fit returned error flag 2: fit is too inaccurate');
            end
        end
        % increase cntr_filename
        cntr_filename = cntr_filename+1;
        disp([callerID 'Continuing loop, now searching for trialID ' num2str(cntr_filename)]);
    end
    pause(loopPause);
end
disp([callerID 'Done ' mfilename]);
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
    
    if isfield(xml.VP, 'condition')
        s.VP.condition = str2double(xml.VP.condition.Text);
    else
        s.VP.condition = 0;
    end;
    
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

%% copy data files to tmp folder
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

data = loadBROSExperimentData(tmpDir,tmpDir,[],0);

% select last 20 seconds of data
fldnms = fieldnames(data.trial(end));
for ii = 1:length(fldnms)
    x = data.trial(end).(fldnms{ii});
    data.trial(end).(fldnms{ii}) = x(end-20000+1:end,:);
end

% select data from last trial only
data = data.trial(end);



% %% import data
% % load all data
% alldata = importTCdata(tmpDir,'model_base_bros');
% keyboard
% % extract trials
% idxtrial = findseq(double(alldata.ExpTrialNumber == trialID & alldata.ExpTrialRunning));
% idx = idxtrial(1,2):idxtrial(1,3);
% keyboard
% data = struct;
% params = fieldnames(alldata);
% 
% for ii = 1:length(params)
%     data.(params{ii}) = alldata.(params{ii})(idx,:);
% end
% 
% % add time vector
% t = alldata.time(idx); t = t - t(1);
% data.t = t;

cd(currentdir);

end