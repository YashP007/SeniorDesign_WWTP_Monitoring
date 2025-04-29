%% Advanced Optical Transmission with Iterative Summation & Cell Blocking
% This model uses a summation block to iteratively calculate photon transmission
% layer-by-layer, starting from the first layer and moving towards the detector.
% It accounts for the polycarbonate transmission after the last layer of cells.

% Clear previous variables and close figures
clear; clc; close all;

%% Define Constants & Model Parameters
phot_emission_avg = 90; % Average photon generation rate per cell (photons/sec)
phot_emission_std = (200 - 20) / 2; % Standard deviation based on range
cell_height = 1e-4; % Cell height in cm (1 um)
cell_density = 1e6; % Cell density in cells/mL
sample_vol = 2e-3; % Sample volume in mL
total_cells = cell_density * sample_vol; % Total number of cells
abs_coeff_cell = 0.05; % Absorption coefficient of cells (cm^-1)

% Well dimensions in cm
well_length = 0.2; % Well length in cm
well_width = 0.2; % Well width in cm
max_well_height = 0.5; % Maximum well height in cm
height_step = 0.05; % Step size for height variation (cm)
well_heights = 0:height_step:max_well_height; % Array of well heights

% Material properties for polycarbonate
abs_polycarb = 0.1; % Absorption coefficient (cm^-1)

% Detector parameters for different sensing elements
detector_area = 0.1; % Detector active area (cm^2)
solid_angle_coverage = 10*detector_area / (4 * pi * (well_length/2)^2); % Simplified solid angle

% Quantum efficiencies for different detectors
qe_photodiode = 0.6; % Photodiode
qe_phototransistor = 100*0.7; % Phototransistor
qe_cmos = 0.4; % CMOS p+/n-well photodiode

e_charge = 1.602e-19; % Elementary charge (Coulombs)

%% Loop Through Well Heights & Calculate Photocurrent with Summation
% Preallocate arrays for storing results
current_photodiode_avg = zeros(size(well_heights));
current_phototransistor_avg = zeros(size(well_heights));
current_cmos_avg = zeros(size(well_heights));
current_photodiode_std = zeros(size(well_heights));
current_phototransistor_std = zeros(size(well_heights));
current_cmos_std = zeros(size(well_heights));

for i = 1:length(well_heights)
    well_height = well_heights(i); % Current well height
    layers = round(well_height / cell_height); % Number of cell layers

    % Initial photon emission for average, min, and max cases
    I_avg = phot_emission_avg * solid_angle_coverage;
    I_min = (phot_emission_avg - phot_emission_std) * solid_angle_coverage;
    I_max = (phot_emission_avg + phot_emission_std) * solid_angle_coverage;

    % Iterative summation block to account for each cell layer
    photons_avg = 0; % Average photon count reaching polycarb
    photons_min = 0; % Min photon count reaching polycarb
    photons_max = 0; % Max photon count reaching polycarb
    for j = 1:layers
        % Calculate the remaining number of photons after each cell layer
        photons_avg = photons_avg + I_avg * exp(-abs_coeff_cell * (layers - j + 1) * cell_height);
        photons_min = photons_min + I_min * exp(-abs_coeff_cell * (layers - j + 1) * cell_height);
        photons_max = photons_max + I_max * exp(-abs_coeff_cell * (layers - j + 1) * cell_height);
    end

    % Transmission through polycarbonate
    trans_polycarb = exp(-abs_polycarb * well_height);
    photons_avg = photons_avg * trans_polycarb;
    photons_min = photons_min * trans_polycarb;
    photons_max = photons_max * trans_polycarb;

    % Calculate photocurrent for each detector
    % Photodiode
    current_photodiode_avg(i) = photons_avg * qe_photodiode * e_charge;
    current_photodiode_std(i) = std([photons_min, photons_max]) * qe_photodiode * e_charge;

    % Phototransistor
    current_phototransistor_avg(i) = photons_avg * qe_phototransistor * e_charge;
    current_phototransistor_std(i) = std([photons_min, photons_max]) * qe_phototransistor * e_charge;

    % CMOS p+/n-well photodiode
    current_cmos_avg(i) = photons_avg * qe_cmos * e_charge;
    current_cmos_std(i) = std([photons_min, photons_max]) * qe_cmos * e_charge;
end

%% Plot Photocurrent vs. Well Height for Each Detector Type
figure;
hold on;

% Set bold and large text properties
set(gca, 'FontSize', 16, 'FontWeight', 'bold');

% Plot data with bold lines and error bars
plot(well_heights, current_photodiode_avg * 1e12, 'r', 'LineWidth', 3); % Photodiode
plot(well_heights, current_phototransistor_avg * 1e12, 'g', 'LineWidth', 3); % Phototransistor
plot(well_heights, current_cmos_avg * 1e12, 'b', 'LineWidth', 3); % CMOS

% Set labels with bold text
xlabel('Well Height (cm)', 'FontSize', 18, 'FontWeight', 'bold');
ylabel('Photocurrent (pA)', 'FontSize', 18, 'FontWeight', 'bold');
title('Photocurrent vs. Well Height for Different Photodetectors', 'FontSize', 20, 'FontWeight', 'bold');

% Configure legend with large and bold font
legend({'Photodiode', 'Phototransistor', 'CMOS p+/n-well'}, 'FontSize', 16, 'FontWeight', 'bold', 'Location', 'best');

% Add grid for better readability
grid on;

yscale('log')

% Set axis lines thicker for better visibility
set(gca, 'LineWidth', 1.5);

hold off;

%% Comments on Results
% The plot shows the photocurrent generated as a function of well height
% for different photodetectors, considering the average photon emission rate 
% with ±1 standard deviation. As well height increases, more cell layers contribute
% to photon absorption, reducing the number of photons reaching the polycarbonate 
% and detector.
%
% - The phototransistor generally generates the highest current due to its
%   higher quantum efficiency.
% - The CMOS p+/n-well photodiode produces the lowest current, indicating
%   lower sensitivity.
% - Standard deviation lines show the variability in photocurrent, helping 
%   understand the impact of photon emission rate variability on detector response.
