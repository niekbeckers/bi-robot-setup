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
resultstoragepath = [resultspath 'results-modelfit-' datestr(now,'ddmmyy-HHMM')];
if ~exist(resultstoragepath,'dir')
    mkdir(resultstoragepath);
end
settingsstoragepath = [settingspath 'settings-modelfit-' datestr(now,'ddmmyy-HHMM')];
if ~exist(settingsstoragepath,'dir')
    mkdir(settingsstoragepath);
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
        f = getlatestfiles([settingspath settings_filename '*.xml']);
        mysettingsfile = f(end).name;
        [loadOkay,s] = readXML([settingspath mysettingsfile]);
    catch me
        loadOkay = false;
    end
    
    if loadOkay
        if isfield(s.VP, 'terminate')
            keepRunning = false;
            continue; % go to next iteration of while loop, skip everything below
        end
        errorFlag = 0;
        
        disp([callerID 'Loaded ' mysettingsfile ', starting model fit']);
        
        % initialize and prepare stuff
        fitIDs = s.VP.doFitForBROSID(:);
        trialID = s.VP.trialID;
        condition = s.VP.condition;
        
        % load data of trial with trialID
        clear data
        data = loadTrialData(datapath);
        
        % select data for optim function
        ds = 10;
        dataArray = NaN(length(data.t(1:ds:end)),8,length(fitIDs));
        t = data.t(1:ds:end,:);
        dt = round(mode(diff(t)),2);
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
        % add fitIDs to output
        for ii = 1:length(fitIDs)
            resultsmodelfit.VP.doFitForBROSID.(['id' num2str(ii-1)]) = fitIDs(ii);
        end
        
        
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
                p0(:,ii) = normrnd(p0_saved(:,id), (ub-lb)/50);
                p0(:,ii) = min(max(p0(:,ii),lb),ub); % bound p0 by upper and lower bound
%                 p0(:,ii) = [normrnd(p0_saved(1,id),ub(1)/12); normrnd(p0_saved(2,id),ub(2)/12); normrnd(p0_saved(3,id),ub(3)/12)];
            else 
                p0(:,ii) = lb+rand(3,1).*(ub-lb);  
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
        resultsmodelfit.VP.iterations.fitInfo = num2cell(fitInfo);
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
            pfit_opt(:,id) = pfit_all(:,I);
            for jj = 1:size(pfit_opt,1)
                resultsmodelfit.VP.modelparameters.(['bros' num2str(id)]).(['x' num2str(jj)]) = pfit_opt(jj,id);
            end
            
            % store optimal fit as p0 for next optimization
            p0_saved(:,id) = pfit_opt(:,id);
        end
        catch me
            disp(me)
            keyboard
        end
       
        % store data in mat file (regardless of fiterror)
        outputfile = [resultspath 'results_vpmodelfit_trial' num2str(resultsmodelfit.VP.trialID)];
        save([outputfile '.mat'],'resultsmodelfit','dataArray'); % save to mat files
        movefile([outputfile '.mat'],resultstoragepath); % copy to output file store
        
        % move settingsfile to storage
        movefile([settingspath mysettingsfile],settingsstoragepath);
        
        if (errorFlag == 0)
            % write results to XML file (and store mat file)
            writeXML(resultsmodelfit,[outputfile '.xml']);
            copyfile([outputfile '.xml'],resultstoragepath);
            disp([callerID 'Results written to ''results_vpmodelfit_trial' num2str(resultsmodelfit.VP.trialID) '.xml/mat''']);
        else
            % model fit threw error
            switch(errorFlag)
                case 1
                    disp('Model fit returned error flag 1: system is unstable');
                case 2
                    disp('Model fit returned error flag 2: fit is too inaccurate, error difference is too high');
                case 3
                    disp('Model fit returned error flag 3: fit is too inaccurate, VAF values are low');
            end
        end
        % increase cntr_filename
        cntr_filename = cntr_filename+1;
        disp([callerID 'Continuing loop, searching for the next settings file']);
    end
    pause(loopPause);
end
disp([callerID 'Done ' mfilename]);
end

function [loadOkay,s] = readXML(filename)
%% function [loadOkay,s] = readXML(filename)

loadOkay = false;
s = struct;

% select newest filename filename with highest trial number
f = getlatestfiles(filename);
filename = [f(end).folder filesep f(end).name];

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
catch me
    disp(me);
    keyboard
    writeOkay = false;
end

end

function [data] = loadTrialData(datapath)
%% function data = loadTrialData(datapath,trialID)

nrTrialsLookback = 5; % select last 5 data files,
currentdir = pwd;
cd(datapath);                                   % go to data directory

%% copy data files to tmp folder
datafiles = getlatestfiles('data_part*.mat',nrTrialsLookback);
filenames = {datafiles(:).name};
tmpDir = 'tmpDirDataModelFit';
% copy files to folder
if ~exist(tmpDir,'dir')
    mkdir(tmpDir); 
else
    % clear any data files from this directory
    delete(fullfile(cd, [tmpDir filesep 'data_part*.mat']));
end
% copy the new data files to the tmpDir
cellfun(@(x)copyfile(x,tmpDir), filenames);

% load experiment data
alldata = loadBROSExperimentData(tmpDir,'savedata',false);


% select last trial only (latest/newest trial)
data = alldata.trial(end);

% select last 20 seconds of data
Nsel = 20000;
fldnms = fieldnames(data);
x = data.(fldnms{1});
if (size(x,1) < Nsel)
    NselOld = Nsel;
    Nsel = size(x,1);
    warning(['size(x,1) = ' num2str(size(x,1)) ' < ' num2str(NselOld) ', setting Nsel to ' num2str(Nsel) '.']);
end

for ii = 1:length(fldnms)
    x = data.(fldnms{ii});
    data.(fldnms{ii}) = x(end-Nsel+1:end,:);
end

% select data from last trial only
% data = data.trial(end);

cd(currentdir);

end

function latestfiles = getlatestfiles(directory,nrfiles)
%This function returns the latest file from the directory passsed as input
%argument

if nargin < 2
    nrfiles = 1;
end

%Get the directory contents
dirc = dir(directory);

%Filter out all the folders.
dirc = dirc(~cellfun(@isdir,{dirc(:).name}));

%I contains the index to the biggest number which is the latest file
[~,I] = sort([dirc(:).datenum]);

if nrfiles > length(I)
    nrfiles = length(I);
end

% select latest nrfiles
I = I(end-nrfiles+1:end);

if ~isempty(I)
    latestfiles = dirc(I);
end

end