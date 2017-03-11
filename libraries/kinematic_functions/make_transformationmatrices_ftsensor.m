%% make_TransformationMatrices_FTSensor

% transformation matrices
syms alpha q1 q2 q3 q4 q5
alpha = pi - q2; % if the sensor is attached to the other side: use q5 and alpha = -(pi-q4)
R1 = [cos(q1) -sin(q1) 0; sin(q1) cos(q1) 0; 0 0 1];
R2 = [cos(alpha) -sin(alpha) 0; sin(alpha) cos(alpha) 0; 0 0 1];
R3 = [cos(-0.5*pi) -sin(-0.5*pi) 0;sin(-0.5*pi) cos(0.5*pi) 0; 0 0 1];
R = inv(R3*R2*R1);
R = blkdiag(R,R); % transform forces and moments
matlabFunction(R,'Vars',{[q1;q2;q3;q4;q5]},'File','func_transformationMatrix_ftsensor_1');

alpha = -(pi - q4); % if the sensor is attached to the other side: use q5 and alpha = -(pi-q4)
R1 = [cos(q5) -sin(q5) 0; sin(q5) cos(q5) 0; 0 0 1];
R2 = [cos(alpha) -sin(alpha) 0; sin(alpha) cos(alpha) 0; 0 0 1];
R3 = [cos(-0.5*pi) -sin(-0.5*pi) 0;sin(-0.5*pi) cos(0.5*pi) 0; 0 0 1];
R = inv(R3*R2*R1);
R = blkdiag(R,R); % transform forces and moments
matlabFunction(R,'Vars',{[q1;q2;q3;q4;q5]},'File','func_transformationMatrix_ftsensor_2');

clear R R1 R2 R3 q1 q2 q3 q4 q5 alpha