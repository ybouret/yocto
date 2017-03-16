species =
{
    { "H+",   1 , 1e-2},
    { "HO-", -1 },
    { "AcH",  0 },
    { "Ac-",  -1 }, { "Na+",   1 }, { "Cl-",   -1 }
};

function Kv(t)
return 10^(-4.8)*(1.0+0.01*math.cos(t))
end

eqs =
{
    { "acetic", "Kv" , { 1, "H+" }, {1,"Ac-"}, {-1,"AcH"} },
    { "water", 1e-14 , { 1, "H+"}, {1, "HO-"} }
};

C0=0;

sol =
{
    { "E/N" },
    { 0, {1,"Na+"} },
    { 0, {1,"Cl-"} },
    { C0, {1,"AcH"}, {1,"Ac-"} }
};
