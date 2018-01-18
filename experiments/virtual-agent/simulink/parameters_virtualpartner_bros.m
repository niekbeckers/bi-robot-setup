%% base_1fm_parameters

%% 'globals'
sampleTime = 0.001;             % sample time [s] Note: make sure the same sample time is set in TwinCAT XAE.
fs = 1/sampleTime;              % sampling frequency
fn = fs/2;                      % nyquist frequency
Lf = 0.238;                     % forearm length [m]
Lu = 0.153;                     % upperarm length [m]
Lb = 0.07;                      % base width [m]


%% target data
% load target signal data
load('data_target_signal.mat','nx','ny','Ax','Ay','phx','phy');

% butterworth filter (filtering velocity signal) 
fc = 60;
[Bbutter,Abutter] = butter(2, fc/fn);

% force field
FFMatrix = -[0 -15; 15 0]; % added minus due to coordinate system flip (y pointing up)

% butterworth filter (filtering velocity signal) 
fc1 = 1.7;
fc2 = 0.2;
[NoiseFiltB1,NoiseFiltA1] = butter(3, fc1/fn);
[NoiseFiltB2,NoiseFiltA2] = butter(1, fc2/fn, 'high');

%% virtual partner settings
dt_vp = 0.01;
VP.dt = dt_vp;   % the virtual partner runs at 100Hz
VP.tp = 0.2;    % prediction horizon
VP.td = 0.2;    % neural time delay 

VP.x0 = zeros(10,1);

% dynamics matrices
tu = 0.04;
D = 0*[0 15;-15 0]; % should be turned on if FF is active! >how though. this script has no inputs.
gamma = 0.8;        % should be 0.8, but unstable with m = 0.3kg
m_vp = diag([1 1]); % see Todorov, 2007

[Ae_vp,B_vp,H_vp] = dynamics_vp(dt_vp,m_vp,tu,dt_vp,0,D);
Aim_vp            = dynamics_vp(dt_vp,m_vp,tu,dt_vp,0,gamma*D);

VP.m = m_vp;
VP.Ae = Ae_vp;
VP.Aim = Aim_vp;
VP.B = B_vp;
VP.H = H_vp;

% process noise
sigmaU_Ow = 0.002/sqrt(0.01)*sqrt(dt_vp);
sigmaT_Ow = 0.002/sqrt(0.01)*sqrt(dt_vp);
sigmaTV_Ow = 0.0005/sqrt(0.01)*sqrt(dt_vp);
Ow = zeros(size(VP.Ae));
Ow(5,5) = sigmaU_Ow^2;
Ow(6,6) = sigmaU_Ow^2;
Ow(7,7) = sigmaT_Ow^2;
Ow(8,8) = sigmaT_Ow^2;
Ow(9,9) = sigmaTV_Ow^2;
Ow(10,10) = sigmaTV_Ow^2;
VP.Ow = Ow;

% sensory/observation noise
sigmaP_Ov = 0.05*0.005/sqrt(0.01)*sqrt(dt_vp);
sigmaV_Ov = 0.05*0.005/sqrt(0.01)*sqrt(dt_vp);
sigmaT_Ov = 0.05*0.005/sqrt(0.01)*sqrt(dt_vp);
sigmaTV_Ov = 0.01*0.005/sqrt(0.01)*sqrt(dt_vp);
Ov = diag([sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2 sigmaT_Ov^2 sigmaT_Ov^2 sigmaTV_Ov^2 sigmaTV_Ov^2]);
VP.Ov = Ov;

% Initialize S0 (initial state uncertainty)
VP.S0 = initializeS0(VP.Ae,VP.Aim,VP.H,Ow,Ov,10000);

%% RobotStruct BROS1
% clear BROS1

BROS1.HomeLocationOpSpace = [0;0.25];                     % home location (homing) [m]
BROS1.HomeLocationSize = 0.0025;
BROS1.NominalPositionOpSpace = [0;0.25];

BROS1.DynModParams = [9.446018e-04  5.536255e-04  1.102026e-01  1.175426e-01 -6.234047e-03  1.037213e-01  3.853734e-03]';
% actuator 1 data
BROS1.Actuator1.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
BROS1.Actuator1.AbsEncoderOffset = 54769/BROS1.Actuator1.JointAbsoluteEncoderCounts_rev*2*pi + asin(8/153);                     % offset absolute encoder [rad]
BROS1.Actuator1.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
% FM1.Actuator1.FOAWNoiseLevel = 4*2*pi/FM1.Actuator1.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
BROS1.Actuator1.FOAWNoiseLevel = 9e-4;                  % Noise level for FOAW algorithm
BROS1.Actuator1.transmissionRatio = 7.31011;               % transmission ratio (estimated)[-]
BROS1.Actuator1.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
BROS1.Actuator1.drivePeakCurrent = 20;                    % drive peak current [A]
BROS1.Actuator1.AbsoluteMaximumTorque_Nm = 12;             % maximum allowable torque (at joints) [Nm]
BROS1.Actuator1.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
BROS1.Actuator1.JointPositionGuardMin = -25*pi/180;       % maximum allowable joint position [rad]
BROS1.Actuator1.JointPositionGuardMax = 135*pi/180;       % maximum allowable joint position [rad]
BROS1.Actuator1.FilterButterA = Abutter;
BROS1.Actuator1.FilterButterB = Bbutter;

% actuator 2 data
BROS1.Actuator2.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
BROS1.Actuator2.AbsEncoderOffset = 44338/BROS1.Actuator2.JointAbsoluteEncoderCounts_rev*2*pi + pi - asin(8/153);                     % offset absolute encoder [rad]
BROS1.Actuator2.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
% FM1.Actuator2.FOAWNoiseLevel = 4*2*pi/FM1.Actuator2.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
BROS1.Actuator2.FOAWNoiseLevel = 9e-4; % Noise level for FOAW algorithm
BROS1.Actuator2.transmissionRatio = 7.32236;               % transmission ratio (estimated) [-]
BROS1.Actuator2.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
BROS1.Actuator2.drivePeakCurrent = 20;                    % drive peak current [A]
BROS1.Actuator2.AbsoluteMaximumTorque_Nm = 12;             % maximum allowable torque (at joints) [Nm]
BROS1.Actuator2.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
BROS1.Actuator2.JointPositionGuardMin = 45*pi/180;        % maximum allowable joint position [rad]
BROS1.Actuator2.JointPositionGuardMax = 205*pi/180;       % maximum allowable joint position [rad]
BROS1.Actuator2.FilterButterA = Abutter;
BROS1.Actuator2.FilterButterB = Bbutter;

% force/torque sensor data
BROS1.FTSensor.MaxAllowableForcesTorqueSensor = 0.7*[100 100 200 2 2 2]'; % maximum allowable forces and torques
BROS1.FTSensor.TransformationMatrixSide = 1; % 1 = transformation matrix based from q1, 2 = transformation from q5.

%% RobotStruct_FM2
% clear FM2

BROS2.HomeLocationOpSpace = [0;0.25];                     % home location (homing) [m]
BROS2.HomeLocationSize = 0.0025;
BROS2.NominalPositionOpSpace = [0;0.25];


% FM2.DynModParams = [1.597109e-03  1.580786e-03  7.028260e-02  7.988736e-02 0*-9.954503e-04  1.006366e-01 0*-1.994173e-03]';                     % dynamic model parameters (I1,I2,mehat,Fs1,Fv1,Fs2,Fv2)
BROS2.DynModParams = [1.117256e-03 1.224600e-03 8.800769e-02 1.073326e-01 1.462254e-03 1.416925e-01 6.135432e-03]';

% actuator 1 data
BROS2.Actuator1.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
BROS2.Actuator1.AbsEncoderOffset = 33205/BROS2.Actuator1.JointAbsoluteEncoderCounts_rev*2*pi + asin(8/153);                     % offset absolute encoder [rad]
BROS2.Actuator1.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
% FM2.Actuator1.FOAWNoiseLevel = 4*2*pi/FM1.Actuator1.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
BROS2.Actuator1.FOAWNoiseLevel = 9e-4; % Noise level for FOAW algorithm
BROS2.Actuator1.transmissionRatio = 7.36678;               % transmission ratio (estimated)[-]
BROS2.Actuator1.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
BROS2.Actuator1.drivePeakCurrent = 20;                    % drive peak current [A]
BROS2.Actuator1.AbsoluteMaximumTorque_Nm = 12;             % maximum allowable torque (at joints) [Nm]
BROS2.Actuator1.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
BROS2.Actuator1.JointPositionGuardMin = -25*pi/180;       % maximum allowable joint position [rad]
BROS2.Actuator1.JointPositionGuardMax = 135*pi/180;       % maximum allowable joint position [rad]
BROS2.Actuator1.FilterButterA = Abutter;
BROS2.Actuator1.FilterButterB = Bbutter;

% actuator 2 data
BROS2.Actuator2.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
BROS2.Actuator2.AbsEncoderOffset = 14143/BROS2.Actuator2.JointAbsoluteEncoderCounts_rev*2*pi + pi - asin(8/153);                     % offset absolute encoder [rad]
BROS2.Actuator2.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
% FM2.Actuator2.FOAWNoiseLevel = 4*2*pi/FM1.Actuator2.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
BROS2.Actuator2.FOAWNoiseLevel = 9e-4; % Noise level for FOAW algorithm
BROS2.Actuator2.transmissionRatio = 7.33339;               % transmission ratio (estimated) [-]
BROS2.Actuator2.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
BROS2.Actuator2.drivePeakCurrent = 20;                    % drive peak current [A]
BROS2.Actuator2.AbsoluteMaximumTorque_Nm = 12;             % maximum allowable torque (at joints) [Nm]
BROS2.Actuator2.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
BROS2.Actuator2.JointPositionGuardMin = 45*pi/180;        % maximum allowable joint position [rad]
BROS2.Actuator2.JointPositionGuardMax = 205*pi/180;       % maximum allowable joint position [rad]
BROS2.Actuator2.FilterButterA = Abutter;
BROS2.Actuator2.FilterButterB = Bbutter;

% force/torque sensor data
BROS2.FTSensor.MaxAllowableForcesTorqueSensor = 0.8*[100 100 200 2 2 2]'; % maximum allowable forces and torques
BROS2.FTSensor.TransformationMatrixSide = 2; % 1 = transformation matrix based from q1, 2 = transformation from q5.