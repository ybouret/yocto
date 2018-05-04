species =
{
    { "H+", 1 },
    { "HO-", -1},
    { "AH",  0},
    { "Am", -1}
};

Kw = 1e-14;

function Ka(t)
return 10^(-4.8);
end

cs =
{
    { "water", Kw , { 1, "H+"}, {1,"HO-"} },
    { "acetic", "Ka", { 1, "H+" }, { 1, "Am"}, {-1,"AH"} }
};

