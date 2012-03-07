Islandora Solr Views
====================

DO NOT USE! THIS MODULE IS IN PROTOTYPE STAGE!

Exposes Islandora Solr Search results into a drupal view.

Usage
-----
Use with:
- views 3
- islandora_solr_search D7: https://github.com/Islandora/islandora_solr_search/tree/7.x (is a prototype as well)


Inspired by
-----------
apachesolr_views: http://drupal.org/project/apachesolr_views



Done - @TODO: get proper data structure cached with luke.
Done: - @TODO: fix pager. Use views settings
@TODO: Move luke function into islandora_solr_search API
@TODO: Pull in human readable names (and help texts?) for islandora solr views handlers
@TODO: fix query
@TODO: fix results rendering. empty results, arrays, etc..
@TODO: provide handler fields
@TODO: provide handler filtering
@TODO: provide handler contextual filtering
@TODO: provide handler sorting
@TODO: automatically replaced the dots in default values of exposed filter parameters. Maybe that should be added on form validate too.
@TODO: (exposed) filters only work with AND statements, not with OR.



/solr/select?start=0
&rows=2
&&fl=label,content,entity_id,id,ss_name,ds_created,ss_language,im_vid_2,tid,id,entity_id,entity_type,bundle,bundle_name,label,is_comment_count,ds_created,ds_changed,score,path,url,is_uid,tos_name,tm_node
&qf=taxonomy_names^2.0
&qf=label^5.0
&qf=content^40
&qf=tos_name^3.0
&qf=tos_content_extra^0.1
&facet=true
&facet.sort=count
&facet.mincount=1
&facet.field=im_field_tags
&facet.field=im_field_content_model
&facet.field=is_uid
&facet.field=ss_language
&facet.field=bundle
&f.im_field_tags.facet.limit=50
&f.im_field_tags.facet.mincount=1
&f.im_field_content_model.facet.limit=50
&f.im_field_content_model.facet.mincount=1
&facet.date=ds_changed
&facet.date=ds_created
&f.ds_changed.facet.date.start=2012-02-26T00:00:00Z/DAY
&f.ds_changed.facet.date.end=2012-03-02T00:00:00Z+1DAY/DAY
&f.ds_changed.facet.date.gap=+1DAY
&f.ds_changed.facet.limit=50
&f.ds_created.facet.date.start=2012-02-26T01:54:00Z/MINUTE
&f.ds_created.facet.date.end=2012-02-26T02:24:00Z+1MINUTE/MINUTE
&f.ds_created.facet.date.gap=+1MINUTE
&f.ds_created.facet.limit=50
&f.is_uid.facet.limit=50
&f.is_uid.facet.mincount=1
&f.ss_language.facet.limit=50
&f.ss_language.facet.mincount=1
&f.bundle.facet.limit=50
&f.bundle.facet.mincount=1
&wt=json&json.nl=map 