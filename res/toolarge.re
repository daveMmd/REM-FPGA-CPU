^\/[a-f0-9]+000[a-f0-9]{37}$
规则膨胀原因：
初看与/^ES1@*#{n}ES2/相似（注：@表示某个字符/字符集，#表示某个字符集）
实则与/&*@#{n}/情况相似，因为@包含于#且#包含于&,[a-f0-9]+扮演了&*的角色

^Host\x3a[^\r\n]+ebay\.com[^\r\n]{20,}\r\n
规则膨胀原因与上一条类似，不过DFA minimise可生效，因为/[^\r\n]{20,}/类似于/#{n,}/


^Host\x3a\x20[^\r\n]+tsb\.co\.uk\.personal\.logon\.login\.jsp\.[a-z0-9.]{50,}
规则膨胀原因与前两条类似：
一个有趣的现象，在.*和#{n}之间的字符数（不管重叠与否）越多，会降低规则膨胀度
-->单条规则的膨胀是由.* 和 #{n}之间的奇妙关系决定的

^[\r\n\s]*=[\r\n\s]*[\x22\x27][^\x22\x27]+\/[a-zA-Z0-9\/\-\_]{60,}\/[a-zA-Z0-9]+\.exe\?[a-zA-Z0-9]+=[a-zA-Z0-9]+(&h=\d+)?[\x22\x27]
^Host\x3a[^\r\n]+cartasi[^\r\n]{20,}\r\n
^Host\x3a[^\r\n]+usaa\.com[^\r\n]{20,}\r\n
^[a-zA-Z0-9\/\+]+OjgwL[a-zA-Z0-9\/\+]{19}[\x22\x27]
^PRIVMSG\s+[^\s]+\s+\x3a\s*\x01SENDLINK\x7c[^\x7c]{69}
^[^\r\n]*filename\s*=\s*[^\x3b\x3a\r\n]*[\x25\x24]\s*\{[^\r\n]{20,}\}

#(fbit-dga too large)
^[a-z0-9]{23}[a-f0-9]{33}.[a-z0-9\-_]+.[a-z0-9\-_]+\x00\x00\x01\x00\x01