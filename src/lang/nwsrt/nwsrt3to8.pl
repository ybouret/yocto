use Algorithm::Networksort;

for (my $i = 3; $i <= 8; $i++) {
    my $inputs = $i;
    my $algorithm = "bosenelson";

    #
    # Generate the sorting network (a list of comparators).
    #
    my $nw = Algorithm::Networksort->new(inputs =>$inputs,algorithm => $algorithm);

    #
    # Print the comparator list using the default format,
    # and print a graph of the list.
    #
    print $nw->formatted(), "\n";
    #print $nw->graph_text(), "\n";

    #
    # Set up a pretty SVG image.
    #
    #$nw->graphsettings(vt_margin => 8, vt_sep => 13, hz_sep=>15);
    #$nw->colorsettings(foreground => "#206068", background => "#c8c8c8");
    #print $nw->graph_svg();
}
