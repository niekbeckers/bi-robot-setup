function dataout = calculate_parameters(datain, expprotocol, varargin)
%% calculate experiment parameters:
%
% Data structure
% Data (RMSe, improvement, relative performance) is parsed into blocks 


% parse inputs
p = inputParser;
p.addOptional('Nsel',2^16);
p.addOptional('dt',0.001);
p.addOptional('AlignTrials',false);
p.parse(varargin{:});

% select inputparser results
Nsel = p.Results.Nsel;
dt = p.Results.dt;

% determine which trials are connected connected
connected = [expprotocol.block.connected].*([expprotocol.block.connectionStiffness] > 0);
Nblocks = size(connected,2);

% trial indices (in blocks)
idx_trials = reshape(1:numel(connected),size(connected));
idx_connected = reshape(connected == 1,size(connected));
idx_single = reshape(connected == 0,size(connected));

% preallocate
rmse1 = NaN(size(connected));
rmse2 = NaN(size(connected));
blocks = struct;

for ii = 1:Nblocks % blocks
    
    % preallocate. 2 partners * number of trials
%     ft_x_all = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     x_x_all  = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     e_x_all  = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     u_x_all  = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     ft_y_all = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     x_y_all  = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     e_y_all  = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     u_y_all  = NaN(size(datain.trial(1).target_BROS1,1),2*sum(~isnan(idx_trials(:,ii))));
%     t = NaN(size(datain.trial(1).target_BROS1,1),1);

    % preallocate. 2 partners * number of trials
    ft_x_all = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    x_x_all  = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    e_x_all  = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    u_x_all  = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    ft_y_all = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    x_y_all  = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    e_y_all  = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    u_y_all  = NaN(Nsel,2*sum(~isnan(idx_trials(:,ii))));
    t = NaN(Nsel,1);
    
    % per trial and partner, calculate RMS
    for jj = 1:size(idx_trials,1)
        idxtrial = idx_trials(jj,ii);
        dat = datain.trial(idxtrial);
        
        % select target and cursor data
        target = NaN(Nsel,4);
        cursor = NaN(Nsel,4);
        
        % number of data points, time step
        N = size(dat.target_BROS1,1);
        if (N > Nsel)
            idx = N-Nsel+1:N;
        else
            idx = 1:N;
        end

  
        %%% time traces
        ft_x = [dat.target_BROS1(idx,1) dat.target_BROS2(idx,1)];
        x_x = [dat.cursor_BROS1(idx,1) dat.cursor_BROS2(idx,1)];
        u_x = [dat.x_BROS1(idx,1) dat.x_BROS2(idx,1)];
        e_x = ft_x-x_x;
        
        ft_y = [dat.target_BROS1(idx,2) dat.target_BROS2(idx,2)];
        x_y = [dat.cursor_BROS1(idx,2) dat.cursor_BROS2(idx,2)];
        u_y = [dat.x_BROS1(idx,2) dat.x_BROS2(idx,2)];
        e_y = ft_y-x_y;
        t = dat.time(idx); t = t-t(1);
        
        % append ft_all etc (combine trials columns per block)
        idx2 = (jj-1)*2+(1:2);
        ft_x_all(:,idx2) = ft_x;
        x_x_all(:,idx2) = x_x;
        e_x_all(:,idx2) = e_x;
        u_x_all(:,idx2) = u_x;
        
        ft_y_all(:,idx2) = ft_y;
        x_y_all(:,idx2) = x_y;
        e_y_all(:,idx2) = e_y;
        u_y_all(:,idx2) = u_y;
        
        target(end-length(idx)+1:end,:) = [dat.target_BROS1(idx,:) dat.target_BROS2(idx,:)];
        cursor(end-length(idx)+1:end,:) = [dat.cursor_BROS1(idx,:) dat.cursor_BROS2(idx,:)];
        
        if p.Results.AlignTrials
            % shift target and cursor data such that for each trial they are
            % the same (i.e. trialRandomization to 0)
            Tshift = expprotocol.block(ii).trialRandomization(jj);
            Tshift = round(Tshift/dt)*dt;
            Nshift = round(Tshift/dt);
            target = circshift(target,Nshift,1);
            cursor = circshift(cursor,Nshift,1);
        end
        
        % calculate RMS
        rmse1(jj,ii) = calculate_performance(target(:,1:2),cursor(:,1:2));
        rmse2(jj,ii) = calculate_performance(target(:,3:4),cursor(:,3:4));
    end
    
%     % fix some errors (due to TC errors/restarts)
%     [idx_connected(:,ii),idx_single(:,ii),rmse1(:,ii),rmse2(:,ii),rmse1_parts(:,ii,:),rmse2_parts(:,ii,:)] = ...
%         fixerrors(pairID,ii,idx_connected(:,ii),idx_single(:,ii),rmse1(:,ii),rmse2(:,ii),rmse1_parts(:,ii,:),rmse2_parts(:,ii,:));
    
    if ~any(idx_connected(:)==1)
        idx_connected(2:2:end,:) = true;
        idx_single(2:2:end,:) = false;
    end
    
    % time traces
    blocks(ii).ft_x = ft_x_all;
    blocks(ii).x_x = x_x_all;
    blocks(ii).e_x = e_x_all;
    blocks(ii).u_x = u_x_all;
    blocks(ii).ft_y = ft_y_all;
    blocks(ii).x_y = x_y_all;
    blocks(ii).e_y = e_y_all;
    blocks(ii).u_y = u_y_all;
    blocks(ii).t = t;
    
    % store data per block
    blocks(ii).rmse = [rmse1(:,ii) rmse2(:,ii)];
    
    blocks(ii).rmse_single = NaN(size(blocks(ii).rmse));
    blocks(ii).rmse_single(idx_single(:,ii),:) = blocks(ii).rmse(idx_single(:,ii),:);
   
    blocks(ii).rmse_connected = NaN(size(blocks(ii).rmse));
    blocks(ii).rmse_connected(idx_connected(:,ii),:) = blocks(ii).rmse(idx_connected(:,ii),:);
    
    blocks(ii).idx_trials = repelem(idx_trials(:,ii),1,2); % relative idx_trial indices
    
    blocks(ii).idx_trials_connected = NaN(size(blocks(ii).idx_trials));
    blocks(ii).idx_trials_connected(idx_connected(:,ii),:) = repelem(idx_trials(idx_connected(:,ii),ii),1,2); % relative idx_trial_connected indices
    
    blocks(ii).idx_trials_single = NaN(size(blocks(ii).idx_trials));
    blocks(ii).idx_trials_single(idx_single(:,ii),:) = repelem(idx_trials(idx_single(:,ii),ii),1,2); % relative idx_trial_connected indices
    
    blocks(ii).idx_connected = repelem(idx_connected(:,ii),1,2);
    blocks(ii).idx_single = repelem(idx_single(:,ii),1,2);

    % calculate HHI improvement and relative performance
    if (ii==2) || (ii==3)
        idx_rows_connected = blocks(ii).idx_trials_connected(:,1) - blocks(ii).idx_trials(1) + 1; idx_rows_connected(isnan(idx_rows_connected)) = [];
        idx_rows_single = idx_rows_connected + 1;
        [blocks(ii).improvement_hhi, blocks(ii).relperformance_hhi] = calculate_hhi_improvement(blocks(ii).rmse,idx_rows_connected,idx_rows_single);
    else
        blocks(ii).improvement_hhi = NaN(4,2);
        blocks(ii).improvement_hhi = NaN(4,2);  
    end


end

% store all data
% dataout.subjID = subjID;
dataout.blocks = blocks;
dataout.rmse = [blocks(:).rmse];
dataout.rmse_single = [blocks(:).rmse_single];
dataout.rmse_connected = [blocks(:).rmse_connected];
dataout.idx_trials = repelem(idx_trials,1,2);
dataout.idx_trials_connected = [blocks(:).idx_trials_connected];
dataout.idx_trials_single = [blocks(:).idx_trials_single];
dataout.idx_connected = repelem(idx_connected,1,2);
dataout.idx_single = repelem(idx_single,1,2);
dataout.idx_blocks = repelem(repmat(1:Nblocks,size(connected,1),1),1,2);
dataout.idx_partner = repmat(repmat(1:2,size(connected,1),1),1,Nblocks);

% % improvement (all, force field (blocks 2 & 3), baseline (block 1), washout (block 4)
% dataout.improvement_hhi = [blocks(:).improvement_hhi];
% dataout.relperformance_hhi = [blocks(:).relperformance_hhi];

end

function [rmse] = calculate_performance(t,x)
%% [rmse,rmse_part,idx_trialsparts] = calculate_performance(t,x,idx_trials,Nparts)
% calculate root-mean-squared (RMS) of the tracking error as a performance
% measure across the complete trial or the trial chopped into Nparts equal parts

% RMSe across complete trial
e = sqrt(sum((t-x).^2,2));

% histogram(e); hold on
% plot([mean(e) mean(e)],ylim,'k','linewidth',2)
% plot([rms(e) rms(e)],ylim,'k--','linewidth',2)
% plot([median(e) median(e)],ylim,'k:','linewidth',2)
% plot([geomean(e) geomean(e)],ylim,'k.-');

rmse = rms(e);

% % calculate RMS across trial in equal parts
% e_parts = reshape(e,[round(length(e)/Nparts) Nparts]);
% rmse_part = rms(e_parts,1);
% 
% % trial numbersmean
% idx_trialsparts = NaN([size(idx_trials),Nparts]);
% for ii = 1:Nparts
%     idx_trialsparts(:,:,ii) = idx_trials+(ii-1)*(1/Nparts);
% end

end

% function [idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts] = fixerrors(subjnr,blocknr,idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts)
% %% function [idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts] = fixerrors(subjnr,idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts)
% 
% % % subject 1
% % if (subjnr == 1) && (sum(idx_connected) > 10)
% %     % if per block more than 10 connected trials are found, I made an
% %     % error in the experiment protocol. 
% %     idx_connected = [idx_connected(2:end); false];
% %     idx_single = [idx_single(2:end); true];
% %     % fix the rmse -  set data points to NaN
% %     rmse1 = [rmse1(2:end); NaN];
% %     rmse2 = [rmse2(2:end); NaN];
% %     rmse1_parts = [rmse1_parts(2:end,:,:); NaN(size(rmse1_parts(end-1,:,:)))];
% %     rmse2_parts = [rmse2_parts(2:end,:,:); NaN(size(rmse2_parts(end-1,:,:)))];
% % end
% % 
% % % pair 18: trial 1 of block 2, session 1 started before we were back at the
% % % controls. Use data from trial 2, sicne this was the actual first trial.
% % % (solo pair)
% % if (subjnr == 18) && (blocknr == 2) % && (rmse1(1) > 0.02) && (rmse2(1) > 0.02)
% %     rmse1(1) = rmse1(2);
% %     rmse2(1) = rmse2(2);
% % end
% 
% end

