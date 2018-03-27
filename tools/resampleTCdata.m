function [tres,datares] = resampleTCdata(t,data,dt)
%% function [tres,datares] = resampleTCData(t,data)
% This function resamples the data to a fixed data rate (defined by dt).
% Apparently the TCExtendedFileWriter of TwinCAT skips data points when
% writing closing old/opening new mat files.


if nargin < 3
    dt = 0.001;
end

% make sure the matrices etc are column-wise
if size(t,2) > size(t,1)   
    t = t.';
end

if length(t) == size(data,2)
    data = data.';
end

tres = (t(1):dt:t(end)).'; % new (resampled) time vector

% resample using interp1
try 
    [t,idx] = unique(t);
    data = data(idx,:);
    datares = interp1(t,data,tres,'linear','extrap');
catch me
    disp(me)
    keyboard
%     datares = interp1(unwrap(t),data,tres,'linear','extrap');
end
end

