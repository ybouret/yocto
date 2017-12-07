use Algorithm::Networksort;
use Algorithm::Networksort::Best qw(:all);

for (my $i = 9; $i <= 16; $i++)
{
    my $inputs = $i;

    #
    # First find if any networks exist for the size you want.
    #
    my @nwkeys = nw_best_names($inputs);

    #
    # For each sorting network, show the comparators.
    #
    for my $name (@nwkeys)
    {
        my $nw = nwsrt_best(name => $name);

        #
        # Print the list, and print the graph of the list.
        #
        print $nw->title(), "\n";
        print $nw->formatted(), "\n\n";
        #print $nw->graph_text(), "\n\n";
    }
}
