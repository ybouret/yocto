species =
{
    { "H+",   1 , 1e-2},
    { "HO-", -1 },
    { "AcH",  0 },
    { "Ac-",  -1 },
    { "Na+",   1 },
    { "Cl-",   -1 }
};

function Kv(t)
return 10^(-4.8)*(1.0+0.01*math.cos(t))
end

eqs =
{
    { "water", 1e-14 , { 1, "H+"}, {1, "HO-"} },
    { "acetic", "Kv" , { 1, "H+" }, {1,"Ac-"}, {-1,"AcH"} }
};

sol =
{
    { "E/N" },
    { "I", 0.1 },
    { 0, {1,"Na+"} }
};
