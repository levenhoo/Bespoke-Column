<?php

/**
 * @see bespoke_column_page_menu
 */
function _bespoke_column_page() {   
  $tax = isset($_GET['tax'])?$_GET['tax']:'';
  $data = isset($_GET['data'])?$_GET['data']:'';

 /* $date = isset($_GET['date'])?(int)$_GET['date']:0; */ 

 /* if(strlen($date)==5 && is_int($date)){
    $preDate = substr($date,0,4);
    $season = substr($date,4,5);   
    $beginTime="0";
    $endTime="0";
     switch ($season) {
        case '0':
          $beginTime=mktime(0, 0, 0, 1, 1, $preDate);
          $endTime=mktime(0, 0, 0, 1, 1, ($preDate+1));
          break;
        case '1':
          $beginTime=mktime(0, 0, 0, 1, 1, $preDate);
          $endTime=mktime(0, 0, 0, 4, 1, $preDate);
          break;
        case '2':
          $beginTime=mktime(0, 0, 0, 4, 1, $preDate);
          $endTime=mktime(0, 0, 0, 7, 1, $preDate);
          break;
        case '3':
          $beginTime=mktime(0, 0, 0, 7, 1, $preDate);
          $endTime=mktime(0, 0, 0, 10, 1, $preDate);
          break;
        case '4':
          $beginTime=mktime(0, 0, 0, 10, 1, $preDate);
          $endTime=mktime(0, 0, 0, 1, 1, ($preDate+1));
          break;
        default:        
          break;
      }     
  }*/

  $nodeQuery = new EntityFieldQuery();
   $entities = $nodeQuery->entityCondition('entity_type', 'node')
   ->entityCondition('bundle',BESPOKE_COLUMN_PAGE_CONTENT_TYPE)//content type 
   ->propertyOrderBy('nid','desc');  
  $entities = $nodeQuery-> pager(10);

  /*if(!empty($beginTime)&&!empty($endTime)){
    $entities = $nodeQuery ->propertyCondition('created', $beginTime , '>=');
    $entities = $nodeQuery ->propertyCondition('created', $endTime , '<=');
  }*/
  if(!empty($tax))
  $entities = $nodeQuery ->fieldCondition('field_'.BESPOKE_COLUMN_PAGE_CONTENT_TYPE_FIELD_MACHINE_NAME, 'tid', $tax);
  if(!empty($data)) 
  $entities = $nodeQuery ->propertyCondition('nid', $data);
  $entities = $nodeQuery-> execute();
  
  $rows = array();
  $nids = array();
  if($entities)
    foreach($entities['node'] as $entity){
      $node = node_load($entity->nid);
      $nids[] = $node->nid;
      
      $rows[] = array(
          $node->nid,
          $node->title
      );
  }

  if(!$nids){
    return  t("<p class='no_post'>沒有相關文章</p>");
  }
  
  $nodes = node_load_multiple($nids);//讀取NID數組獲得NODE數組 

  /*
   * 2014-7-24 update
   */
  $new_nodes = array();
  foreach ($nodes as $key => $node) {
    
    //Send to all affiliates 
    if($node->domain_site==true){
      $new_nodes[] = $node;
      continue;
    } 
    //Send to Bespoke Column
    if(array_search('16', $node->domains))
    {
       $new_nodes[] = $node;
    } 
  }
  //-----------------


  $nodes_view = node_view_multiple($new_nodes);//NODE数组组成NODE_VIEW
  $output = render($nodes_view);
  $output .= theme('pager');
  return $output;
  
}

/*
 * @see bespoke_column_page_block_view
 * top 10 news
 */
function _bespoke_column_page_top10news(){


 /* $query = db_select('node', 'n')
  ->fields('n')
  ->condition('n.type',BESPOKE_COLUMN_PAGE_CONTENT_TYPE)//content type 
  ->orderBy('n.created', 'desc')
  ->range(0,10)
  ->execute()
  ->fetchAllAssoc("nid"); */

   /*
   * 2014-7-24 update
   */
  $arg = array( 
    ':type'=> BESPOKE_COLUMN_PAGE_CONTENT_TYPE ,
    ':gid1' => 16 , 
    ':gid2' => 0 ) ;
  $sql = 'SELECT DISTINCT n.nid ,  n.title FROM {node} n JOIN {domain_access} d on d.nid = n.nid where  ( n.type = :type  and d.gid = :gid1 ) or  ( n.type = :type  and d.gid = :gid2 ) order By n.created desc  ';
  $result = db_query_range( $sql, 0 , 10 ,$arg);
  //--------------------------------

  $html = '<ul>';
  $data = isset($_GET['data'])?(int)$_GET['data']:0;  
  /*foreach ($result as $row) {
    if($data==$row->nid)
    $html .='<li><a class="current_article" href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row->nid.'">'.$row->title.'</a></li>';   
    else
    $html .='<li><a href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row->nid.'">'.$row->title.'</a></li>';   
  }*/
   while ( $row = $result->fetchAssoc() ) {        
      if($data==$row['nid'])
         $html .='<li><a class="current_article" href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row['nid'].'">'.$row['title'].'</a></li>';   
      else
      $html .='<li><a href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row['nid'].'">'.$row['title'].'</a></li>';   
    } 

  $html .= '</ul>';
  return $html;
}

/*
 * @see bespoke_column_page_block_view
 * column vocabularies
 */
function _bespoke_column_page_get_vocabularies(){
  $vocabularies = taxonomy_get_vocabularies(); 
  foreach ($vocabularies as  $vocabulary) {
    
    if($vocabulary->machine_name==BESPOKE_COLUMN_PAGE_TAXONOMY_MACHINE_NAME){
      $query = taxonomy_get_tree($vocabulary->vid);
      break;
    }   
  }   
  if(isset($query)){    
    $html = '<ul>';
    $tax = isset($_GET['tax'])?(int)$_GET['tax']:0;  
    foreach ($query as $row) {
      if($tax==$row->tid){
        $html .='<li><a class="current_taxonomy" href="/'.BESPOKE_COLUMN_PAGE.'/?tax='.$row->tid.'">'.$row->name.'</a></li>';   
      }else{
        $html .='<li><a href="/'.BESPOKE_COLUMN_PAGE.'/?tax='.$row->tid.'">'.$row->name.'</a></li>';   
      }
    }
    $html .= '</ul>';    
    return $html; 
  }
}

/*
 *  return post list
 *  
 */
function _bespoke_column_page_get_post_bydate($date){ 

   if(strlen($date)==5 && is_int($date)){
    $preDate = substr($date,0,4);
    $season = substr($date,4,5);   
    $beginTime="0";
    $endTime="0";
     switch ($season) {
        case '0':
          $beginTime=mktime(0, 0, 0, 1, 1, $preDate);
          $endTime=mktime(0, 0, 0, 1, 1, ($preDate+1));
          break;
        case '1':
          $beginTime=mktime(0, 0, 0, 1, 1, $preDate);
          $endTime=mktime(0, 0, 0, 4, 1, $preDate);
          break;
        case '2':
          $beginTime=mktime(0, 0, 0, 4, 1, $preDate);
          $endTime=mktime(0, 0, 0, 7, 1, $preDate);
          break;
        case '3':
          $beginTime=mktime(0, 0, 0, 7, 1, $preDate);
          $endTime=mktime(0, 0, 0, 10, 1, $preDate);
          break;
        case '4':
          $beginTime=mktime(0, 0, 0, 10, 1, $preDate);
          $endTime=mktime(0, 0, 0, 1, 1, ($preDate+1));
          break;
        default:        
          break;
      }     
  }else{
    return '';
  } 
 /*  $query = db_select('node', 'n')
  ->fields('n')
  ->condition('n.type',BESPOKE_COLUMN_PAGE_CONTENT_TYPE)//content type
  ->condition('n.created',$beginTime,'>=')
  ->condition('n.created',$endTime,'<=')
  ->orderBy('n.created', 'desc')
  ->range(0,10)
  ->execute()
  ->fetchAllAssoc("nid"); */

  /*
   * 2014-7-24 update
   */
   $arg = array( 
    ':type'=> BESPOKE_COLUMN_PAGE_CONTENT_TYPE ,
    ':gid1' => 16 , 
    ':gid2' => 0 ,
    ':beginTime' => $beginTime,
    ':endTime' => $endTime
    ) ;
  $sql = 'SELECT DISTINCT n.nid ,  n.title , n.created FROM {node} n JOIN {domain_access} d on d.nid = n.nid where  (( n.type = :type  and d.gid = :gid1 ) or  ( n.type = :type  and d.gid = :gid2 )) and (n.created >= :beginTime and n.created <= :endTime ) order By n.created desc  ';
  $result = db_query_range( $sql, 0 , 100 ,$arg); 
  //-----------------

  
  $html = '';
  $data = isset($_GET['data'])?(int)$_GET['data']:0;  
 /* foreach ($query as $row) {
    if ($data==$row->nid)
      $html .='<dd class="postlist"><a class="current_article" href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row->nid.'&date='.$_GET['date'].'">'.$row->title.'</a></dd>'; 
    else
      $html .='<dd class="postlist"><a href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row->nid.'&date='.$_GET['date'].'">'.$row->title.'</a></dd>'; 
  } */
   while ( $row = $result->fetchAssoc() ) {        
     if ($data==$row['nid'])
      $html .='<dd class="postlist"><a class="current_article" href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row['nid'].'&date='.$_GET['date'].'">'.$row['title'].'</a></dd>'; 
     else
      $html .='<dd class="postlist"><a href="/'.BESPOKE_COLUMN_PAGE.'?data='.$row['nid'].'&date='.$_GET['date'].'">'.$row['title'].'</a></dd>'; 
   } 
  return $html;
}

/*
 * @see bespoke_column_page_block_view
 * column vocabularies
 */
function _bespoke_column_page_date_feedback(){ 
  /*$query = db_select('node', 'n')
  ->fields('n')
  ->condition('n.type',BESPOKE_COLUMN_PAGE_CONTENT_TYPE)//content type
  ->orderBy('n.created', 'desc')
  ->range(0,1)
  ->execute()
  ->fetchAllAssoc("nid");  
  foreach ($query as $entity) {  	 
  	$year = date('Y',$entity->created);  	
  }*/ 


  /*
   * 2014-7-24 update
   */
   $arg = array( 
    ':type'=> BESPOKE_COLUMN_PAGE_CONTENT_TYPE ,
    ':gid1' => 16 , 
    ':gid2' => 0 ) ;
  $sql = 'SELECT DISTINCT n.nid ,  n.title , n.created FROM {node} n JOIN {domain_access} d on d.nid = n.nid where  ( n.type = :type  and d.gid = :gid1 ) or  ( n.type = :type  and d.gid = :gid2 ) order By n.created desc  ';
  $result = db_query_range( $sql, 0 , 1 ,$arg);

  while ( $row = $result->fetchAssoc() ) {     
    $year = date('Y',$row['created']); 
  } 
  //-----------------

  $dateList = array();
   $thisMonth = date("n");  
   for ($i=date('Y') ; $i >= $year ; $i--) { 
     $dateList[$i.'0']='<dt class="year"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'0">'.$i.'</a></dt>';
     if($i!=date('Y')){
        $dateList[$i.'4'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'4">10-12月</a></dd>'; 
        $dateList[$i.'3'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'3">7-9月</a></dd>';
        $dateList[$i.'2'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'2">4-6月</a></dd>';           
        $dateList[$i.'1'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'1">1-3月</a></dd>';
     }else{
        switch ($thisMonth) {
          case '10':
          case '11':
          case '12':         
            $dateList[$i.'4'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'4">10-12月</a></dd>'; 
            $dateList[$i.'3'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'3">7-9月</a></dd>';
            $dateList[$i.'2'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'2">4-6月</a></dd>';           
            $dateList[$i.'1'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'1">1-3月</a></dd>';
            break;          
          case '7':
          case '8':
          case '9':        
            $dateList[$i.'3'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'3">7-9月</a></dd>';
            $dateList[$i.'2'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'2">4-6月</a></dd>';           
            $dateList[$i.'1'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'1">1-3月</a></dd>';
            break;
          case '4':
          case '5':
          case '6':
            $dateList[$i.'2'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'2">4-6月</a></dd>';           
            $dateList[$i.'1'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'1">1-3月</a></dd>';      
            break;
          case '1':
          case '2':
          case '3':           
            $dateList[$i.'1'] ='<dd class="month"><a href="/'.BESPOKE_COLUMN_PAGE.'?date='.$i.'1">1-3月</a></dd>';                  
            break;    
          default:
            # code...
            break;
        }
     }
   }  
  /*
    2014/6/5
    click month display post list
  */
  $date = isset($_GET['date'])?(int)$_GET['date']:0;  
  if(strlen($date)==5 && is_int($date)){ 
    $postList = _bespoke_column_page_get_post_bydate($date);
    foreach ($dateList as $key => $value) {      
      if($date==$key){
          //匹配時間把POST加到點擊的月份下方
          $value = str_replace('class="month"', 'class="month current_month"', $value);
          $dateList[$key]=$value.$postList;
      }
    }
  }   
  $html ='';
  $html .= implode("", $dateList);
  return '<dl>'.$html.'</dl>'; 
}