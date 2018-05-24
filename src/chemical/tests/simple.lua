species =
{
    { "H+",   1},
    { "HO-", -1},
    { "AH",   0},
    { "Am",  -1},
    { "NH4+", 1},
    { "NH3",  0},
    { "Na+",  1},
    { "Cl-", -1},
    { "OxH2", 0},
    { "OxH-", -1},
    { "Ox--", -2}
};

Kw = 1e-14;

function Ka(t)
return 10^(-4.8)*(1.0+0.1*math.sin(6.28*t/60.0));
end

Kb = 10^(-9.2);

cs =
{
    { "water", Kw , { 1, "H+"}, {1,"HO-"} },
    { "acetic", "Ka", { 1, "H+" }, { 1, "Am"}, {-1,"AH"} } ,
    { "ammoniac", Kb, { 1, "H+" }, { 1, "NH3" }, {-1,"NH4+"} },
    { "Oxalic1", 10^(-1.2), {1,"H+"}, {1,"OxH-"}, {-1,"OxH2"} },
    { "Oxalic2", 10^(-4.3), {1,"H+"}, {1,"Ox--"}, {-1,"OxH-"} }

};

math.randomseed(os.time());
amplitude=0.01;

Cb  = amplitude*math.random();
Ca  = amplitude*math.random();
COx = amplitude*math.random();

ini =
{
    { Ca,    { 1, "AH"   },  { 1, "Am"  } },
    { Cb,    { 1, "NH4+" },  { 1, "NH3" } },
    { COx,   { 1, "OxH2" },  { 1, "OxH-"}, {1, "Ox--" } },
    { "E/N" },
    { 0.0, { 1,"Na+"} },
    { Cb, { 1,"Cl-"} }
}

add =
{
    { "E/N" },
    { 0,    { 1, "AH"   },  { 1, "Am"  } },
    { 0,    { 1, "NH4+" },  { 1, "NH3" } },
    { 0,    { 1, "OxH2" },  { 1, "OxH-"}, {1, "Ox--" } },
    { 0.01, { 1, "Na+" } },
    { 0,    { 1, "Cl-" } }
};
