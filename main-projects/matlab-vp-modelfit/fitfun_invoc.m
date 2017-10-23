function sse = fitfun_invoc(params,xmeas,target,doFF)

% asserts (for mex)
assert(isa(params,'double')); assert(all(size(params)==[1 3]));
assert(isa(xmeas,'double')); coder.varsize('xmeas',[4, Inf],[0,1]);
assert(isa(target,'double')); coder.varsize('target',[4, Inf],[0,1]);
assert(isa(doFF,'double')); assert(all(size(doFF)==[1 1]));


%% simulate lqg
[xe] = sim_lqg(target,doFF);

%% calculate error

error_human = sqrt((target(1,:)-xmeas(1,:)).^2+(target(2,:)-xmeas(2,:)).^2);
error_agent = sqrt((target(1,:)-xe(1,:)).^2+(target(2,:)-xe(2,:)).^2);

% to get the root-mean-squared error of the agent and human equal
e_rms = (rms(error_human) - rms(error_agent));

% to get trajectories/fit to be similar (sum-squared error)
e_fit = sum((error_human-error_agent).^2);

% sse
sse = e_fit + 10*e_rms;



