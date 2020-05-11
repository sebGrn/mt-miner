$.getJSON('tree-data.json', function(treeData) {
   // get the tree-container
    var treeDiv = document.getElementById("tree");

    // set the margin of the tree
    var margin = {
        top: 20, 
        right: 50, 
        bottom: 20, 
        left: 50
    };

    // variables
    var i = 0;
    var duration = 500;
    var root;

    // assigns parent, children, height, depth
    root = d3.hierarchy(treeData, function(d) { 
        return d.children; 
    });

    // get the maximum depth of the tree 
    getDepth = function(node) {
        var depth = 0;
        if(node.children) {
            node.children.forEach(function(d) {
                var tmpDepth = getDepth(d);
                if(tmpDepth > depth) {
                    depth = tmpDepth;
                }
            })
        }
        return 1 + depth
    }
    var depth = getDepth(root);

    // set the dimensions of the tree
    var width = treeDiv.clientWidth - margin.left - margin.right;
    var height = depth*180 - margin.top;

    // append the svg object to the tree-container of the page
    // appends a 'group' element to 'svg'
    // moves the 'group' element to the top left margin
    var svg = d3.select(treeDiv).append("svg")
    .attr("width", (width + margin.right + margin.left))
    .attr("height", (height + margin.top + margin.bottom))
    .attr("role", "img")
    .attr("viewBox", "0 0 " + (width + margin.right + margin.left) + " " + (height + margin.top + margin.bottom))
    .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    // declares a tree layout and assigns the size
    var treemap = d3.tree().size([width, height]);

    // set the position of the tree root 
    root.x0 = 0;
    root.y0 = width / 2;

    // collapse after the second level
    root.children.forEach(collapse);
    update(root);
    
    // resize the window width
    function resize(d) {
        width = (treeDiv.clientWidth - margin.left - margin.right);
        d3.select("svg")
        .attr("width", (width + margin.right + margin.left))
        .attr("height", (height + margin.top + margin.bottom))
        .attr("role", "img")
        .attr("viewBox", "0 0 " + (width + margin.right + margin.left) + " " + (height + margin.top + margin.bottom));
        treemap = d3.tree().size([width, height]);
        root.x0 = 0;
        root.y0 = width / 2;
        update(d);
    }

    // collapse the node and all it's children
    function collapse(d) {
        if(d.children) {
            d._children = d.children;
            d._children.forEach(collapse);
            d.children = null;
        }
    }

    function uncollapse(d) {
        if(d._children) {
            d.children = d._children;
            d.children.forEach(uncollapse);
            d._children = null;
        }
        else {
            if(d.children) {
                d.children.forEach(uncollapse);
            }
        }
    }

    function collapseAll() {
        collapse(root);
        update(root);
    }

    function uncollapseAll() {
        uncollapse(root);
        update(root);
    }

    function update(source) {
        // assigns the x and y position for the nodes
        var treeData = treemap(root);

        // compute the new tree layout
        var nodes = treeData.descendants();
        var links = treeData.descendants().slice(1);

        // normalize for fixed-depth.
        nodes.forEach(function(d) { 
            d.y = d.depth * 180;
        });

        // ****************** Nodes section ***************************
  
        // update the nodes
        var node = svg.selectAll('g.node')
        .data(nodes, function(d) {
            return d.id || (d.id = i++); 
        });

        // enter any new modes at the parent's previous position.
        var nodeEnter = node.enter().append('g')
        .attr('class', 'node')
        .attr("transform", function() {
            return "translate(" + source.x0 + "," + source.y0 + ")";
        })
        .on('click', click);

        // add Circle for the nodes
        nodeEnter.append('circle')
        .attr('class', 'node')
        .attr('r', 1e-6)
        .style("fill", function(d) {
            return d._children ? "lightsteelblue" : "#fff";
        });

        // add labels for the nodes
        nodeEnter.append('text')
        .attr("x", "0")
        .attr("dy", function(d) {
            return d.children || d._children ? -18 : 25;
        })
        .attr("text-anchor", function(d) {
            return d.children || d._children ? "middle" : "middle";
        })
        .text(function(d) { 
            return d.data.name; 
        });


        // UPDATE
        var nodeUpdate = nodeEnter.merge(node);

        // transition to the proper position for the node
        nodeUpdate.transition()
        .duration(duration)
        .attr("transform", function(d) { 
            return "translate(" + d.x + "," + d.y + ")";
        });

        // update the node attributes and style
        nodeUpdate.select('circle.node')
        .attr('r', 10)
        .style("fill", function(d) {
            return d._children ? "lightsteelblue" : "#fff";
        })
        .style("stroke", function(d) {
            var color;
            if(d.data.name[0] == '*') {
                color = "green";
            }
            else {
                color = "steelblue";
            }
            return color;
        })
        .attr('cursor', function(d) {
            var cursor;
            if(!d.children && !d._children) {
                cursor = "default";
            }
            else {
                cursor = "pointer";
            }
            return cursor;
        });


        // remove any exiting nodes
        var nodeExit = node.exit().transition()
        .duration(duration)
        .attr("transform", function(d) {
            return "translate(" + source.x + "," + source.y + ")";
        })
        .remove();

        // on exit reduce the node circles size to 0
        nodeExit.select('circle')
        .attr('r', 1e-6);

        // on exit reduce the opacity of text labels
        nodeExit.select('text')
        .style('fill-opacity', 1e-6);

        // ****************** links section ***************************

        // update the links...
        var link = svg.selectAll('path.link')
        .data(links, function(d) { 
            return d.id; 
        });

        // enter any new links at the parent's previous position.
        var linkEnter = link.enter().insert('path', "g")
        .attr("class", "link")
        .attr('d', function(d){
            var o = {
                x: source.x0, 
                y: source.y0
            }
            return diagonal(o, o)
        });

        // UPDATE
        var linkUpdate = linkEnter.merge(link);

        // transition back to the parent element position
        linkUpdate.transition()
        .duration(duration)
        .attr('d', function(d){ return diagonal(d, d.parent) });

        // remove any exiting links
        var linkExit = link.exit().transition()
        .duration(duration)
        .attr('d', function(d) {
            var o = {
                x: source.x, 
                y: source.y
            }
            return diagonal(o, o)
        })
        .remove();

        // store the old positions for transition.
        nodes.forEach(function(d){
            d.x0 = d.x;
            d.y0 = d.y;
        });

        // creates a curved (diagonal) path from parent to the child nodes
        function diagonal(s, d) {
            path = `M ${s.x} ${s.y}
                    C ${(s.x + d.x) / 2} ${s.y},
                    ${(s.x + d.x) / 2} ${d.y},
                    ${d.x} ${d.y}`

            return path
        }

        // toggle children on click
        function click(d) {
            if (d.children) {
                d._children = d.children;
                d.children = null;
            } 
            else {
                d.children = d._children;
                d._children = null;
            }
            update(d);
        }
    }
    // resize tree on window resize
    window.addEventListener("resize", resize);
    document.getElementById("collapse").addEventListener("click", collapseAll);
    document.getElementById("uncollapse").addEventListener("click", uncollapseAll);
    
});
