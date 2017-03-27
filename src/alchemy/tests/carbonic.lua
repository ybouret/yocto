species =
{
    { "H+",     1 },
    { "HO-",   -1 },
    { "HCO3-", -1 },
    { "CO3--", -2 }
}


-- -----------------------------------------------------------------------------
-- Thermo
-- -----------------------------------------------------------------------------
Kw      = 1.0e-14;
K_Henry = 29.41;   -- atm/(mol/L)

K1      = 4.45e-7;
K2      = 5.6e-11;

function P_CO2(t)
local  P0    = 40.0;
local  W     = 300;
P0 = 0.3;
if (t>=5) and (t<=W+5) then
return P0/760.0 + (8/760.0) * math.sin(math.pi*(t-5)/W)^2;
end
return P0/760;
end

function kappa(t)
local  CO2_aq = P_CO2(t) / K_Henry;
return CO2_aq * K1;
end


eqs =
{
    { "water",  Kw,      {  1, "H+" }, { 1, "HO-"} },
    { "bicarb", "kappa", {  1, "H+" }, { 1, "HCO3-"} },
    { "carb",    K2,     {  1, "H+" }, { 1, "CO3--"}, { -1, "HCO3-" } },
};


sol =
{
    { "E/N" }
};
