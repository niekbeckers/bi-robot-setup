%% parameters_base_1fm

%% 'globals'
sampleTime = 0.001;             % sample time [s] Note: make sure the same sample time is set in TwinCAT XAE.
Lf = 0.238;                     % forearm length [m]
Lu = 0.153;                     % upperarm length [m]
Lb = 0.07;                      % base width [m]

%% RobotStruct_FM1
clear FM1

FM1.HomeLocationOpSpace = [0;0.25];                     % home location (homing) [m]
FM1.HomeLocationSize = 0.01;

FM1.DynModParams = [1 1 1 1 1 1 1];                     % dynamic model parameters (I1,I2,mehat,Fs1,Fv1,Fs2,Fv2)

% actuator 1 data
FM1.Actuator1.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
FM1.Actuator1.AbsEncoderOffset = 0;                     % offset absolute encoder [rad]
FM1.Actuator1.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
FM1.Actuator1.FOAWNoiseLevel = 2*pi/FM1.Actuator1.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
FM1.Actuator1.transmissionRatio = 7.3;                  % transmission ratio [-]
FM1.Actuator1.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
FM1.Actuator1.drivePeakCurrent = 20;                    % drive peak current [A]
FM1.Actuator1.AbsoluteMaximumTorque_Nm = 8;             % maximum allowable torque (at joints) [Nm]
FM1.Actuator1.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
FM1.Actuator1.JointPositionGuardMin = -25*pi/180;       % maximum allowable joint position [rad]
FM1.Actuator1.JointPositionGuardMax = 135*pi/180;       % maximum allowable joint position [rad]

% actuator 2 data
FM1.Actuator2.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
FM1.Actuator2.AbsEncoderOffset = 0;                     % offset absolute encoder [rad]
FM1.Actuator2.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
FM1.Actuator2.FOAWNoiseLevel = 2*pi/FM1.Actuator2.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
FM1.Actuator2.transmissionRatio = 7.3;                  % transmission ratio [-]
FM1.Actuator2.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
FM1.Actuator2.drivePeakCurrent = 20;                    % drive peak current [A]
FM1.Actuator2.AbsoluteMaximumTorque_Nm = 8;             % maximum allowable torque (at joints) [Nm]
FM1.Actuator2.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
FM1.Actuator2.JointPositionGuardMin = 45*pi/180;       % maximum allowable joint position [rad]
FM1.Actuator2.JointPositionGuardMax = 205*pi/180;       % maximum allowable joint position [rad]

% force/torque sensor data
FM1.FTSensor.MaxAllowableForcesTorqueSensor = [100 100 200 2 2 2]*0.7; % maximum allowable forces and torques

% transformation matrices
syms alpha q1 q2 q3 q4 q5
alpha = pi - q2; % if the sensor is attached to the other side: use q5 and alpha = -(pi-q4)
R1 = [cos(q1) -sin(q1) 0; sin(q1) cos(q1) 0; 0 0 1];
R2 = [cos(alpha) -sin(alpha) 0; sin(alpha) cos(alpha) 0; 0 0 1];
R3 = [cos(-0.5*pi) -sin(-0.5*pi) 0;sin(-0.5*pi) cos(0.5*pi) 0; 0 0 1];
R = inv(R3*R2*R1);
R = blkdiag(R,R); % transform forces and moments
FM1.FTSensor.Tmatrix_fcn = matlabFunction(R,'Vars',{[q1;q2;q3;q4;q5]});
clear R R1 R2 R3 q1 q2 q3 q4 q5 alpha



