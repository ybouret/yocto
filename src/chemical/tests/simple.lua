species =
{
    { "H+", 1 },
    { "HO-", -1},
    { "AH",  0},
    { "Am", -1},
    { "NH4+", 1 },
    { "NH3",  0},
    { "Na+", 1},
    { "Cl-", -1}
};

Kw = 1e-14;

function Ka(t)
return 10^(-4.8);
end

Kb = 10^(-9.2);

cs =
{
    { "water", Kw , { 1, "H+"}, {1,"HO-"} },
    { "acetic", "Ka", { 1, "H+" }, { 1, "Am"}, {-1,"AH"} },
    { "ammoniac", Kb^2, { 2, "H+" }, { 2, "NH3" }, {-2,"NH4+"} }
};

