---
layout: page
title: Code
---

I will do my best to include all in-class coding examples here. If there is something missing, email me.

<ul>
{% assign chapters = site.code_chapters | sort: "name" %}
{% for chapter in chapters %}
  <li><a href="{{ chapter.url | relative_url }}">{{ chapter.title }}</a></li>
{% endfor %}
</ul>
