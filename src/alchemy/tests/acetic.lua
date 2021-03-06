species =
{
    { "H+",   1 , 1e-2},
    { "HO-", -1 },
    { "AcH",  0 },
    { "Ac-",  -1 }, { "Na+",   1 }, { "Cl-",   -1 },
    { "NH4+",  1},
    { "NH3",   0}
};

function Kv(t)
return 10^(-4.8)*(1.0+0.01*math.cos(t))
end

eqs =
{
    { "acetic", "Kv" , { 1, "H+" }, {1,"Ac-"}, {-1,"AcH"} },
    { "water", 1e-14 , { 1, "H+"}, {1, "HO-"} },
    { "amoniac", 10^(-9.8) , {1, "H+" }, {1, "NH3"}, {-1,"NH4+"}}
};

C0=0.0001;
C1=0.00001;

sol =
{
    { "E/N" },
    { 0,   {1,"Na+"} },
    { 0,   {1,"Cl-"} },
    { C0,  {1,"AcH"},  {1,"Ac-"} },
    { C1,  {1,"NH4+"}, {1,"NH3"} }
};

function Soude(t,Cin,Cout,params)
local zeta = params["zeta"];
print( "Soude@t=" .. t .. ", zeta=" .. zeta);
local a = {};
local rho = 0.001;
a["Na+"] = rho;
a["HO-"] = rho;
return a;
end

function DoNothing()
a = {};
return a;
end

eff =
{
    "Soude",
    "DoNothing"
};
