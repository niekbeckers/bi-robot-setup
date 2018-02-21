function varargout = parsedata;

%% parsedata
clear all; close all; clc;

pairs = [111];
% outliers = {'10B'};
outliers = {''};

ix = 1;
currdir = [fileparts(mfilename('fullpath')) filesep];

for ii = 1:length(pairs)
    pair = pairs(ii);
    disp(['Parsing pair ' num2str(pair)]);
    try
        % load data and experiment protocol
%             f = dir(['.' filesep 'exp1_motorlearning_partners' num2str(pair) '_type*']);
        f = dir(['.' filesep 'exp2_vp_pilot' num2str(pair) '_type*']);
   
        cd([f(1).folder filesep f(1).name filesep 'data'])
        if exist('data_trials.mat','file')
            load('data_trials.mat');
        else 
            disp('data_trials.mat not found, calling loadBROSExperimentData');
            vars = {'x_BROS1','x_BROS2','xdot_BROS1','xdot_BROS2',...
            'x_AbsEnc_BROS1','x_AbsEnc_BROS2','xdot_AbsEnc_BROS1','xdot_AbsEnc_BROS2',...
            'ForcesOpSpace_BROS1','ForcesOpSpace_BROS2',...
            'target_BROS1','target_BROS2','cursor_BROS1','cursor_BROS2','target_vel_BROS1','target_vel_BROS2',...
            'pos_vp1','pos_vp2'};     
            [data] = loadBROSExperimentData('.','model','model_virtualpartner_bros','vars',vars);
        end

        f = dir(['.' filesep '..' filesep 'protocol_vp_pair' num2str(pair) '*.mat']);
        load([f(1).folder filesep f(1).name]);
        cd(currdir);
            
        % calculate performance metrics
        clear alldatatmp
        alldatatmp = calculate_parameters_vp(data, expprotocol, pair,'Tpart',10);
            
        % remove outliers
        alldatatmp = removeoutliers(alldatatmp,pair,outliers);
            
        alldatatmp.areConnected = any(any([expprotocol.block(:).connected] == 1));
        alldatatmp.pair = pair;

        alldata(ix) = alldatatmp;
        ix = ix + 1;
    catch me
        fprintf('Caught error for pair %d\n',pair)
        disp(getReport(me))
        cd(currdir);
    end
end   
clear alldatatmp;
cd(currdir);

% save data
save('Exp2VPData.mat','alldata');

varargout{1} = alldata;

end

function data = removeoutliers(data,pair,outliers)
%% function data = removeoutliers(data,pair,outliers)
% set data of outlier to NaN

partners = {'A','B'};
for ii = 1:2
    id = [num2str(pair) partners{ii}];
    if any(ismember(id,outliers))
        disp(['Outlier: data for ' num2str(id) ' set to NaN']);
        data = setdatatonan(data,ii);
    end 
end

end

function data = setdatatonan(data,pidx)
%% function data = setdatatonan(data,ii)
% set data corresponding to pidx (partner index, 1 or 2) to NaN for all
% fields in the struct. Also does this recursively for sub-structs in the
% main struct.
fldsdonotnan = {'idx_partner','idx_blocks','idx_trials'};

fldnms = fieldnames(data);
for jj = 1:length(fldnms)
    if ~ismember(fldnms{jj},fldsdonotnan)
        tmp = data.(fldnms{jj});
        if isstruct(tmp)
            for kk = 1:length(tmp)
                tmp(kk) = setdatatonan(tmp(kk),pidx); % recursion, woot!
            end
        else
            if ~any(islogical(tmp)) % cannot set logicals to NaN
                tmp(:,pidx:2:end) = NaN;
            end
        end
        data.(fldnms{jj}) = tmp;
    end
end

end