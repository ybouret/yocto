-- species, level 2

species =
{
    { "H+",   1},
    { "HO-", -1},
    { "AcH",  0},
    { "Ac-", -1},
    { "NH4+", 1},
    { "NH3",  0},
    { "Na+",  1},
    { "Cl-", -1},
    { "CO2",    0},
    { "HCO3-", -1},
    { "CO3--", -2},
};


function Kam(t)
return 10^(-9.1)
end

K1 = 4.45e-7;
K2 = 4.69e-11;

eqs =
{
    { "water",  1e-14,     { 1, "H+"}, { 1, "HO-" } },
    { "acetic", 10^(-4.8), { 1, "H+"}, { 1, "Ac-" }, { -1, "AcH" } },
    { "ammoniac", "Kam",   { 1, "H+"},  { 1, "NH3" }, { -1, "NH4+"} },
    { "K1",    K1,    {-1, "CO2"}, {1, "H+"}, {1, "HCO3-"} },
    { "K2",    K2,    {-1, "HCO3-"}, {1, "H+"}, {1, "CO3--"} }
};

CC = 5e-4;

ini1 =
{
    { "E/N" },
    { 0.001, { 1, "AcH" }, {1, "Ac-" } },
    { 0, {1,"Na+"} },
    { 0, {1,"Cl-"} },
    { 0.00, {1,"NH4+"}, { 1, "NH3" } },
    { CC, { 1, "CO2"}, {1, "HCO3-" }, { 1, "CO3--" } }
};


ini2 =
{
    { "E/N" },
    { 0.00, { 1, "AcH" }, {1, "Ac-" } },
    { 0, {1,"Na+"} },
    { 0, {1,"Cl-"} },
    { 0.001, {1,"NH4+"}, { 1, "NH3" } },
    { 0.0, { 1, "CO2"}, {1, "HCO3-" }, { 1, "CO3--" } }
}

mix =
{
    "ini1",
    "ini2"
};