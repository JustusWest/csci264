---
title: Chapter 2
---

<ul>
{% assign snippets = site.static_files | where_exp: "f", "f.path contains '/code/Chapter_2/'" | sort: "name" %}
{% for snippet in snippets %}
  <li><a href="{{ snippet.path | relative_url }}">{{ snippet.basename }}{{ snippet.extname }}</a></li>
{% endfor %}
</ul>
