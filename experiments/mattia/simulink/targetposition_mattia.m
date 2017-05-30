function [x_pos, y_pos] = targetposition_mattia (trial_randomization, trial_duration) 

% trial randomization is a number from 1 to 20, it indicates which phase
% set to use
%
%
%


load('final_phases_and_functiondata.mat') %load file with amplitudes, nx, ny, and phases sets

fs = 1000;
t = 0:1/fs:trial_duration;
t = t(1:end-1);

w0 = 2*pi/trial_duration; %w0 depends on trial duration

phix = phasesx(:, trial_randomization);
phiy = phasesy(:, trial_randomization);

wx = w0*nx;

iwx = 1j*wx;
taux = 1/3;
Ax_filt2 = abs(1./((taux*iwx+1).^2)); %2nd order filter

wy = w0*ny;

iwy = 1j*wy;
tauy = 1/3;
Ay_filt2 = abs(1./((tauy*iwy+1).^2)); %2nd order filter

x_pos = Ax_filt2(1)*sin((wx(1)*t)+phix(1))+Ax_filt2(2)*sin((wx(2)*t)+phix(2))+Ax_filt2(3)*sin((wx(3)*t)+phix(3))+Ax_filt2(4)*sin((wx(4)*t)+phix(4))+Ax_filt2(5)*sin((wx(5)*t)+phix(5))+Ax_filt2(6)*sin((wx(6)*t)+phix(6));
y_pos = Ay_filt2(1)*sin((wy(1)*t)+phiy(1))+Ay_filt2(2)*sin((wy(2)*t)+phiy(2))+Ay_filt2(3)*sin((wy(3)*t)+phiy(3))+Ay_filt2(4)*sin((wy(4)*t)+phiy(4))+Ay_filt2(5)*sin((wy(5)*t)+phiy(5))+Ay_filt2(6)*sin((wy(6)*t)+phiy(6));

%for i = 1:num_sin
%    x_pos = x_pos + Ax_filt2(i)*sin((wx(i)*t)+phix(i));
%    y_pos = y_pos + Ay_filt2(i)*sin((wy(i)*t)+phiy(i));
%end

% get the max
maxofx = max(abs(x_pos))
maxofy = max(abs(y_pos))
% and normalize
x_pos = x_pos./maxofx;
y_pos = y_pos./maxofy;

x_pos = x_pos';
y_pos = y_pos';

end
