function [data] = importdata()

opts = spreadsheetImportOptions("NumVariables", 4);

% Specify sheet and range
opts.Sheet = "Página1";
opts.DataRange = "A2:D21";

% Specify column names and types
opts.VariableNames = ["DutyCycle", "PWM", "MediaADC", "LuxMedio"];
opts.VariableTypes = ["double", "double", "double", "double"];

% Import the data
LDR0 = readtable("LDR0.xlsx", opts, "UseExcel", false);
LDR1 = readtable("LDR1.xlsx", opts, "UseExcel", false);
LDR2 = readtable("LDR2.xlsx", opts, "UseExcel", false);
LDR3 = readtable("LDR3.xlsx", opts, "UseExcel", false);

data = {LDR0, LDR1, LDR2, LDR3};

end