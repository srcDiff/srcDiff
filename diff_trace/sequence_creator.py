
def power_set(alist) :

    return power_set_helper(alist, 0, len(alist))

def power_set_helper(alist, start, num) :

    if start == len(alist) || num == 0
    return alist

    newlist = alist

    newlist.append(alist, start + 1, num)
    newlist.append(newlist, start +1, num - 1)

    return newlist
