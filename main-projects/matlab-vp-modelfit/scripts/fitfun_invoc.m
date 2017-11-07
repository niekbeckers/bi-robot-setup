function sse = fitfun_invoc(params,dt,xmeas,target,doFF)

% asserts (for mex)
assert(isa(params,'double')); assert(all(size(params)==[3 1]));
assert(isa(xmeas,'double')); coder.varsize('xmeas',[4, Inf],[0,1]);
assert(isa(target,'double')); coder.varsize('target',[4, Inf],[0,1]);
assert(isa(doFF,'double')); assert(all(size(doFF)==[1 1]));
assert(isa(dt,'double')); assert(all(size(dt)==[1 1]));

%% simulate lqg
[xe] = sim_lqg(params,target,dt,doFF,0);

%% calculate error

error_human = sqrt((target(1,:)-xmeas(1,:)).^2+(target(2,:)-xmeas(2,:)).^2);
error_agent = sqrt((target(1,:)-xe(1,:)).^2+(target(2,:)-xe(2,:)).^2);

% to get the tracking error of agent and human equal
e_track = mean(abs(abs(error_human) - abs(error_agent)));

% to get trajectories to be similar
e_fit = mean(abs(error_human-error_agent));

% sse
sse = e_fit + 25*e_track;