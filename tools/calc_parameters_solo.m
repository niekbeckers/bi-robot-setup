function [e, improv_single_trial] = calc_parameters_solo (xtarget, xcursor) 
%
% IN
%
%
% xtarget is the matrix containing position of the target. 1st column is x
% and 2nd column is y * number of trials
%
% xcursor is the matrix containing position of the cursor (subject). 1st
% column is x and 2nd column is y * number of trials
%
%
% trial_sequence is the sequence of trials (dyad=1 or solo=0)
%
%
% OUT
%
%
% e is the tracking error (target - cursor)
%
% improv_dual_trial is the improvement after dual trial
%.
% improv_single_trial is the improvement after single trial
%
% Mattia D'Alessi
% June 2017


inst_e = abs(xtarget - xcursor); % error at every data point
inst_eucl_e = NaN(size(xtarget,1), size(xtarget,2)/2); %calculate eucliden distance (all time points, 60 columns)
eucl_i = 1;

for ii = 1:2:size(xtarget,2)
    inst_eucl_e(:,eucl_i) = sqrt(inst_e(:,ii).^2 + inst_e(:,ii+1).^2);
    eucl_i = eucl_i + 1;
end

e = rms(inst_eucl_e); %here it should be a 1*n_trials vector


    improv_single_trial = NaN(1,length(e)-1);
    
    for ii = 1:(length(e)-1)
        improv_single_trial(ii) = e(ii)-e(ii+1);
    end

end
