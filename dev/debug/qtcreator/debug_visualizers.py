#!/usr/bin/env python
# -*- coding: utf-8 -*-


from dumper import *

def llist_items(value):
    item = value['head']
    i = 0
    while item.pointer():
        yield str(i), item['value']
        i += 1
        item = item['next']

def qdump__LList(d, value):
    items = [(i, v) for i, v in llist_items(value)]
    d.putNumChild(len(items))

    if d.isExpanded():
        with Children(d):
            for i, item in items:
                d.putSubItem(i, item)

