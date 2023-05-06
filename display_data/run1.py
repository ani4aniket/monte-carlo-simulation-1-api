import os
import ipywidgets as widgets
from IPython.display import IFrame

out0 = widgets.Output()


tab = widgets.Tab(children = [out0])
tab.set_title(0, 'Gen9 1024x1024')


display(tab)

with out0:
    display(IFrame(src='reports/Demo1.html', width=1024, height=768))
